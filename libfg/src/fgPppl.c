/*!
 * @file  fgPppl.c
 * @brief Generate Parabola-Parabola-Parabola-Linear (PPPL) functions
 *
 * <h2>Copyright</h2>
 *
 * Copyright CERN 2015. This project is released under the GNU Lesser General
 * Public License version 3.
 * 
 * <h2>License</h2>
 *
 * This file is part of libfg.
 *
 * libfg is free software: you can redistribute it and/or modify it under the
 * terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License
 * for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <string.h>
#include "libfg.h"

enum FG_errno fgPpplInit(struct FG_limits *limits,
                         bool              pol_switch_auto,
                         bool              pol_switch_neg,
                         FG_float          initial_ref,
                         FG_float          acceleration1[FG_MAX_PPPLS],
                         uint32_t          acceleration1_num_els,
                         FG_float          acceleration2[FG_MAX_PPPLS],
                         uint32_t          acceleration2_num_els,
                         FG_float          acceleration3[FG_MAX_PPPLS],
                         uint32_t          acceleration3_num_els,
                         FG_float          rate2        [FG_MAX_PPPLS],
                         uint32_t          rate2_num_els,
                         FG_float          rate4        [FG_MAX_PPPLS],
                         uint32_t          rate4_num_els,
                         FG_float          ref4         [FG_MAX_PPPLS],
                         uint32_t          ref4_num_els,
                         FG_float          duration4    [FG_MAX_PPPLS],
                         uint32_t          duration4_num_els,
                         union  FG_pars   *pars,
                         struct FG_error  *error)
{
    enum   FG_errno fg_errno;                      // Error number
    struct FG_error local_error;                   // Local error data in case user error is NULL
    struct FG_pppl  p;                             // Local PPPL pars - copied to user *pars only if there are no errors
    uint32_t        n_pppls;                       // Number of PPPLs
    uint32_t        pppl_idx;                      // PPPL index (0-(FG_MAX_PPPLS-1))
    uint32_t        seg_idx;                       // Segment index (0-(4*FG_MAX_PPPLS-1))
    uint32_t        num_segs;                      // Total number of segments (4, 8, 12, ...)
    FG_float        time;                          // End of segment times
    FG_float        acc_pow2;                      // Square of acceleration
    FG_float        delta_time  [FG_PPPL_NUM_SEGS];// Segment durations
    FG_float        r           [FG_PPPL_NUM_SEGS];// Reference at start of segment
    FG_float        rate        [FG_PPPL_NUM_SEGS];// Rate of change of at start of segment
    FG_float        acceleration[FG_PPPL_NUM_SEGS];// Acceleration of each segment
    FG_float       *seg_time;                      // Pointer to p.seg_time
    FG_float       *seg_a0;                        // Pointer to p.seg_a0
    FG_float       *seg_a1;                        // Pointer to p.seg_a1
    FG_float       *seg_a2;                        // Pointer to p.seg_a2

    // Reset meta & error structures - uses local_error if error is NULL

    error = fgResetMeta(initial_ref, &p.meta, &local_error, error);

    // Check that number of PPPLs is the same for all seven parameters

    n_pppls = duration4_num_els;

    if(!n_pppls ||
        n_pppls != acceleration1_num_els ||
        n_pppls != acceleration2_num_els ||
        n_pppls != acceleration3_num_els ||
        n_pppls != rate2_num_els         ||
        n_pppls != rate4_num_els         ||
        n_pppls != ref4_num_els)
    {
        fg_errno = FG_BAD_ARRAY_LEN;
        goto error;
    }

    // Prepare to process all PPPLs

    p.seg_idx = 0;
    seg_idx   = 0;
    r[0]      = initial_ref;
    rate[0]   = 0.0;
    time      = 0.0;

    seg_time = &p.seg_time[0];
    seg_a0   = &p.seg_a0  [0];
    seg_a1   = &p.seg_a1  [0];
    seg_a2   = &p.seg_a2  [0];

    // For all PPPLs

    for(pppl_idx=0 ; pppl_idx < n_pppls ; pppl_idx++)
    {
        r[3]    = ref4 [pppl_idx];
        rate[3] = rate4[pppl_idx];
        rate[1] = rate2[pppl_idx];
        acceleration[0] = acceleration1[pppl_idx];
        acceleration[1] = acceleration2[pppl_idx];
        acceleration[2] = acceleration3[pppl_idx];

        // TEST 1: If accelerations or rates are invalid - error.index = 1xx

        if(acceleration[0] == 0.0               ||
           acceleration[2] == 0.0               ||
           acceleration[2] == acceleration[1]   ||
           rate[1]         == rate[0])
        {
            error->index   = 100 + pppl_idx;
            error->data[0] = rate[0];
            error->data[1] = rate[1];
            error->data[2] = acceleration[0];
            error->data[3] = acceleration[2];

            fg_errno = FG_BAD_PARAMETER;
            goto error;
        }

        delta_time[3] = duration4[pppl_idx];
        delta_time[0] = (rate[1] - rate[0]) / acceleration[0];

        r[1] = r[0] + 0.5 * delta_time[0] * (rate[0] + rate[1]);

        acc_pow2 = (2.0 * acceleration[1] * acceleration[2] * (r[3] - r[1]) +
                        rate[1] * rate[1] * acceleration[2] -
                        rate[3] * rate[3] * acceleration[1]) /
                         (acceleration[2] - acceleration[1]);

         // TEST 2: If dB2/delta_time squared is negative - error.index = 2xx

        if(acc_pow2 < 0.0)
        {
            error->index   = 200 + pppl_idx;
            error->data[0] = acc_pow2;
            error->data[1] = r[1];
            error->data[2] = r[3];
            error->data[3] = delta_time[0];

            fg_errno = FG_BAD_PARAMETER;
            goto error;
        }

        rate[2] = sqrt(acc_pow2) * (acceleration[2] > 0.0 ? -1.0 : 1.0);

        // TEST 3: If denominator of delta_time[1] is zero - error.index = 3xx

        if((rate[1] + rate[2]) == 0.0)
        {
            error->index   = 300 + pppl_idx;
            error->data[0] = rate[1];
            error->data[1] = rate[2];
            error->data[2] = rate[1] + rate[2];
            error->data[3] = acc_pow2;

            fg_errno = FG_BAD_PARAMETER;
            goto error;
        }

        delta_time[2] = (rate[3] - rate[2]) / acceleration[2];
        delta_time[1] = (2.0 * (r[3] - r[1]) - delta_time[2] * (rate[2] + rate[3])) / (rate[1] + rate[2]);

        if(delta_time[1] >= 0.0)
        {
            r[2] = r[1] + 0.5 * delta_time[1] * (rate[1] + rate[2]);
        }
        else
        {
            acc_pow2 = (2.0 * acceleration[0] * acceleration[2] * (r[3] - r[0]) +
                            rate[0] * rate[0] * acceleration[2] -
                            rate[3] * rate[3] * acceleration[0]) /
                             (acceleration[2] - acceleration[0]);

            // TEST 4: If dB2/delta_time squared is negative - error.index = 4xx

            if(acc_pow2 < 0.0)
            {
                error->index   = 400 + pppl_idx;
                error->data[0] = acc_pow2;
                error->data[1] = r[1];
                error->data[2] = r[3];
                error->data[3] = delta_time[1];

                fg_errno = FG_BAD_PARAMETER;
                goto error;
            }

            rate[2] = sqrt(acc_pow2) * (acceleration[2] > 0.0 ? -1.0 : 1.0);

            // TEST 5: If denominator of delta_time[0] is zero - error.index = 5xx

            if((rate[0] + rate[2]) == 0.0)
            {
                error->index   = 500 + pppl_idx;
                error->data[0] = rate[0];
                error->data[1] = rate[2];
                error->data[2] = rate[0] + rate[2];
                error->data[3] = acc_pow2;

                fg_errno = FG_BAD_PARAMETER;
                goto error;
            }

            delta_time[2] = (rate[3] - rate[2]) / acceleration[2];
            delta_time[0] = (2.0 * (r[3] - r[0]) - delta_time[2] * (rate[2] + rate[3])) / (rate[0] + rate[2]);
            r[2]          = r[0] + 0.5 *delta_time[0] * (rate[0] + rate[2]);
            delta_time[1] = 0.0;
            rate[1]       = rate[2];
            r[1]          = r[2];
        }

        // TEST 6: If any segments have negative duration

        if(delta_time[0] < 0.0 || delta_time[1] < 0.0 || delta_time[2] < 0.0)
        {
            error->index   = pppl_idx;
            error->data[0] = delta_time[0];
            error->data[1] = delta_time[1];
            error->data[2] = delta_time[2];

            fg_errno = FG_INVALID_TIME;
            goto error;
        }

        time += delta_time[0];
        seg_time[seg_idx] = time;
        seg_a0  [seg_idx] = r[1];
        seg_a1  [seg_idx] = rate[1];
        seg_a2  [seg_idx] = 0.5 * acceleration[0];

        seg_idx++;
        time += delta_time[1];
        seg_time[seg_idx] = time;
        seg_a0  [seg_idx] = r[2];
        seg_a1  [seg_idx] = rate[2];
        seg_a2  [seg_idx] = 0.5 * acceleration[1];

        seg_idx++;
        time += delta_time[2];
        seg_time[seg_idx] = time;
        seg_a0  [seg_idx] = r[3];
        seg_a1  [seg_idx] = rate[3];
        seg_a2  [seg_idx] = 0.5 * acceleration[2];

        fgSetMinMax(seg_a0[seg_idx], &p.meta);

        seg_idx++;
        time += delta_time[3];
        seg_time[seg_idx] = time;
        seg_a0  [seg_idx] = r[3] + rate[3] * delta_time[3];
        seg_a1  [seg_idx] = rate[3];
        seg_a2  [seg_idx] = 0.0;

        r[0]    = seg_a0[seg_idx];
        rate[0] = rate[3];

        fgSetMinMax(seg_a0[seg_idx], &p.meta);

        seg_idx++;
    }

    num_segs   = seg_idx;
    p.num_segs = num_segs;
    p.seg_idx  = 0;

    // Complete meta data

    p.meta.range.final_ref  = seg_a0[num_segs-1];
    p.meta.range.final_rate = seg_a1[num_segs-1];

    fgSetMeta(pol_switch_auto, pol_switch_neg, seg_time[num_segs-1], limits, &p.meta);

    // Check the segments against the limits if provided

    if(limits != NULL)
    {
        for(seg_idx=0 ; seg_idx < num_segs ; seg_idx++)
        {
            fg_errno = fgCheckRef(seg_a0[seg_idx], seg_a1[seg_idx], seg_a2[seg_idx], &p.meta, error);

            if(fg_errno != FG_OK)
            {
                error->index = seg_idx + 1;
                goto error;
            }
        }
    }

    // Copy valid set of parameters to user's pars structure

    memcpy(pars, &p, sizeof(p));

    return(FG_OK);

    // Error - store error code in meta and return to caller

    error:

        error->fg_errno = fg_errno;
        return(fg_errno);
}



enum FG_func_status fgPpplRT(union FG_pars *pars, FG_float func_time, FG_float *ref)
{
    FG_float  seg_time;                // Time within segment

    // If pre-function

    if(func_time < 0.0)
    {
        pars->pppl.seg_idx = 0;
        *ref = pars->pppl.meta.range.initial_ref;

        return(FG_GEN_PRE_FUNC);
    }

    // Scan through the PPPL segments to find segment containing the current time

    while(func_time > pars->pppl.seg_time[pars->pppl.seg_idx])
    {
        // If function complete the coast from last reference and return false

        if(++pars->pppl.seg_idx >= pars->pppl.num_segs)
        {
            pars->pppl.seg_idx = pars->pppl.num_segs - 1;
            *ref          = pars->pppl.seg_a0[pars->pppl.seg_idx];

            return(FG_GEN_POST_FUNC);
        }
    }

    // While time before start of segment - backtrack to the previous segment

    while(pars->pppl.seg_idx > 0 && func_time < pars->pppl.seg_time[pars->pppl.seg_idx - 1])
    {
        pars->pppl.seg_idx--;
    }

    // seg_time is time within the segment

    seg_time = func_time - pars->pppl.seg_time[pars->pppl.seg_idx];

    *ref = pars->pppl.seg_a0[pars->pppl.seg_idx] +
          (pars->pppl.seg_a1[pars->pppl.seg_idx] + pars->pppl.seg_a2[pars->pppl.seg_idx] * seg_time) * seg_time;

    return(FG_GEN_DURING_FUNC);
}

// EOF
