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
#include "libfg/pppl.h"

enum fg_error fgPpplInit(struct   fg_limits *limits, 
                         bool     pol_switch_auto,
                         bool     pol_switch_neg,
                         double   delay, 
                         float    initial_ref,
                         float    acceleration1[FG_MAX_PPPLS],
                         uint32_t acceleration1_num_els,       
                         float    acceleration2[FG_MAX_PPPLS],
                         uint32_t acceleration2_num_els,       
                         float    acceleration3[FG_MAX_PPPLS],
                         uint32_t acceleration3_num_els,       
                         float    rate2        [FG_MAX_PPPLS],
                         uint32_t rate2_num_els,               
                         float    rate4        [FG_MAX_PPPLS],
                         uint32_t rate4_num_els,               
                         float    ref4         [FG_MAX_PPPLS],
                         uint32_t ref4_num_els,                
                         float    duration4    [FG_MAX_PPPLS],
                         uint32_t duration4_num_els,           
                         struct   fg_pppl *pars, 
                         struct   fg_meta *meta)
{
    enum fg_error  fg_error;                      // Error code
    struct fg_meta local_meta;                    // Local meta data in case user meta is NULL
    struct fg_pppl p;                             // Local PPPL pars - copied to user *pars only if there are no errors
    uint32_t       n_pppls;                       // Number of PPPLs
    uint32_t       pppl_idx;                      // PPPL index (0-(FG_MAX_PPPLS-1))
    uint32_t       seg_idx;                       // Segment index (0-(4*FG_MAX_PPPLS-1))
    uint32_t       num_segs;                      // Total number of segments (4, 8, 12, ...)
    float          time;                          // End of segment times
    float          acc_pow2;                      // Square of acceleration
    float          delta_time  [FG_PPPL_NUM_SEGS];// Segment durations
    float          r           [FG_PPPL_NUM_SEGS];// Reference at start of segment
    float          rate        [FG_PPPL_NUM_SEGS];// Rate of change of at start of segment
    float          acceleration[FG_PPPL_NUM_SEGS];// Acceleration of each segment
    float *        segs_t;                        // Pointer to p.t
    float *        segs_a0;                       // Pointer to p.a0
    float *        segs_a1;                       // Pointer to p.a1
    float *        segs_a2;                       // Pointer to p.a2

    meta = fgResetMeta(meta, &local_meta, delay, initial_ref);  // Reset meta structure - uses local_meta if meta is NULL

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
        fg_error = FG_BAD_ARRAY_LEN;
        goto error;
    }

    // Prepare to process all PPPLs

    p.seg_idx  = 0;
    p.initial_ref = initial_ref;
    p.delay    = delay;

    seg_idx = 0;
    r[0]    = initial_ref;
    rate[0] = 0.0;
    time    = 0.0;

    segs_t  = &p.time[0];
    segs_a0 = &p.a0[0];
    segs_a1 = &p.a1[0];
    segs_a2 = &p.a2[0];

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
            meta->error.index   = 100 + pppl_idx;
            meta->error.data[0] = rate[0];
            meta->error.data[1] = rate[1];
            meta->error.data[2] = acceleration[0];
            meta->error.data[3] = acceleration[2];

            fg_error = FG_BAD_PARAMETER;
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
            meta->error.index   = 200 + pppl_idx;
            meta->error.data[0] = acc_pow2;
            meta->error.data[1] = r[1];
            meta->error.data[2] = r[3];
            meta->error.data[3] = delta_time[0];

            fg_error = FG_BAD_PARAMETER;
            goto error;
        }

        rate[2] = sqrt(acc_pow2) * (acceleration[2] > 0.0 ? -1.0 : 1.0);

        // TEST 3: If denominator of delta_time[1] is zero - error.index = 3xx

        if((rate[1] + rate[2]) == 0.0)
        {
            meta->error.index   = 300 + pppl_idx;
            meta->error.data[0] = rate[1];
            meta->error.data[1] = rate[2];
            meta->error.data[2] = rate[1] + rate[2];
            meta->error.data[3] = acc_pow2;

            fg_error = FG_BAD_PARAMETER;
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
                meta->error.index   = 400 + pppl_idx;
                meta->error.data[0] = acc_pow2;
                meta->error.data[1] = r[1];
                meta->error.data[2] = r[3];
                meta->error.data[3] = delta_time[1];

                fg_error = FG_BAD_PARAMETER;
                goto error;
            }

            rate[2] = sqrt(acc_pow2) * (acceleration[2] > 0.0 ? -1.0 : 1.0);

            // TEST 5: If denominator of delta_time[0] is zero - error.index = 5xx

            if((rate[0] + rate[2]) == 0.0)
            {
                meta->error.index   = 500 + pppl_idx;
                meta->error.data[0] = rate[0];
                meta->error.data[1] = rate[2];
                meta->error.data[2] = rate[0] + rate[2];
                meta->error.data[3] = acc_pow2;

                fg_error = FG_BAD_PARAMETER;
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
            meta->error.index   = pppl_idx;
            meta->error.data[0] = delta_time[0];
            meta->error.data[1] = delta_time[1];
            meta->error.data[2] = delta_time[2];

            fg_error = FG_INVALID_TIME;
            goto error;
        }

        time += delta_time[0];
        segs_t [seg_idx] = time;
        segs_a0[seg_idx] = r[1];
        segs_a1[seg_idx] = rate[1];
        segs_a2[seg_idx] = 0.5 * acceleration[0];

        seg_idx++;
        time += delta_time[1];
        segs_t [seg_idx] = time;
        segs_a0[seg_idx] = r[2];
        segs_a1[seg_idx] = rate[2];
        segs_a2[seg_idx] = 0.5 * acceleration[1];

        seg_idx++;
        time += delta_time[2];
        segs_t [seg_idx] = time;
        segs_a0[seg_idx] = r[3];
        segs_a1[seg_idx] = rate[3];
        segs_a2[seg_idx] = 0.5 * acceleration[2];

        fgSetMinMax(meta, segs_a0[seg_idx]);

        seg_idx++;
        time += delta_time[3];
        segs_t [seg_idx] = time;
        segs_a0[seg_idx] = r[3] + rate[3] * delta_time[3];
        segs_a1[seg_idx] = rate[3];
        segs_a2[seg_idx] = 0.0;

        r[0]    = segs_a0[seg_idx];
        rate[0] = rate[3];

        fgSetMinMax(meta, segs_a0[seg_idx]);

        seg_idx++;
    }

    num_segs   = seg_idx;
    p.num_segs = num_segs;
    p.seg_idx  = 0;

    // Complete meta data

    meta->duration         = segs_t [num_segs-1];
    meta->range.end        = segs_a0[num_segs-1];
    meta->range.final_rate = segs_a1[num_segs-1];

    fgSetFuncPolarity(meta, pol_switch_auto, pol_switch_neg);

    // Check the segments against the limits if provided

    if(limits != NULL)
    {
        for(seg_idx=0 ; seg_idx < num_segs ; seg_idx++)
        {
            fg_error = fgCheckRef(limits, segs_a0[seg_idx], segs_a1[seg_idx], segs_a2[seg_idx], meta);

            if(fg_error != FG_OK)
            {
                meta->error.index = seg_idx + 1;
                goto error;
            }
        }
    }

    // Copy valid set of parameters to user's pars structure

    memcpy(pars, &p, sizeof(p));

    return(FG_OK);

    // Error - store error code in meta and return to caller

    error:

        meta->fg_error = fg_error;
        return(fg_error);
}



enum fg_gen_status fgPpplGen(struct fg_pppl *pars, const double *time, float *ref)
{
    double func_time;               // Time within function
    float  seg_time;                // Time within segment

    // Both *time and delay must be 64-bit doubles if time is UNIX time

    func_time = *time - pars->delay;

    // Coast during run delay

    if(func_time < 0.0)
    {
        pars->seg_idx = 0;
        *ref = pars->initial_ref;

        return(FG_GEN_PRE_FUNC);
    }

    // Scan through the PPPL segments to find segment containing the current time

    while(func_time > pars->time[pars->seg_idx])
    {
        // If function complete the coast from last reference and return false

        if(++pars->seg_idx >= pars->num_segs)
        {
            pars->seg_idx = pars->num_segs - 1;
            *ref          = pars->a0[pars->seg_idx];

            return(FG_GEN_POST_FUNC);
        }
    }

    // While time before start of segment - backtrack to the previous segment

    while(pars->seg_idx > 0 && func_time < pars->time[pars->seg_idx - 1])
    {
        pars->seg_idx--;
    }

    // seg_time is time within the segment

    seg_time = func_time - pars->time[pars->seg_idx];

    *ref = pars->a0[pars->seg_idx] +
          (pars->a1[pars->seg_idx] + pars->a2[pars->seg_idx] * seg_time) * seg_time;

    return(FG_GEN_DURING_FUNC);
}

// EOF
