/*!
 * @file  fgTable.c
 * @brief Generate linearly interpolated table functions.
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

#include "string.h"
#include "libfg.h"



enum FG_errno fgTableInit(struct FG_limits *limits,
                          bool              pol_switch_auto,
                          bool              pol_switch_neg,
                          FG_float          min_time_step,
                          FG_float         *ref,
                          uint32_t          ref_num_els,
                          FG_float         *time,
                          uint32_t          time_num_els,
                          FG_float         *armed_ref,
                          FG_float         *armed_time,
                          union  FG_pars   *pars,
                          struct FG_error  *error)
{
    enum   FG_errno fg_errno;       // Error number
    struct FG_error local_error;    // Local error data in case user error is NULL
    struct FG_meta  meta;           // Local TABLE pars - copied to user *pars only if there are no errors
    uint32_t        i;              // loop variable
    uint32_t        num_points;     // Number of points in the table
    FG_float        grad;           // Segment gradient

    // Unlike other functions, we do not need a local copy of struct fg_table.  We just need a local
    // copy of meta because the function doesn't need to write anything other than the meta data
    // to pars until the limits have been checked.

    // Reset meta & error structures - uses local_error if error is NULL

    error = fgResetMeta(ref[0], &meta, &local_error, error);

    // Initial checks of data integrity

    if(ref_num_els < 2 ||                          // If less than 2 points or
       ref_num_els != time_num_els)   // time and ref arrays are not the same length
    {
        error->data[0] = (float)ref_num_els;
        error->data[1] = (float)time_num_els;

        fg_errno = FG_BAD_ARRAY_LEN;                            // Report bad array len
        goto error;
    }

    // Check time vector and calculate min/max for table

    num_points     = ref_num_els;
    min_time_step *= (1.0 - FG_CLIP_LIMIT_FACTOR);      // Adjust min time step to avoid rounding errs

    for(i = 1 ; i < num_points ; i++)
    {
        if(time[i] < (time[i - 1] + min_time_step))        // Check time values
        {
            error->index   = i;
            error->data[0] = time[i];
            error->data[1] = time[i - 1] + min_time_step;
            error->data[2] = min_time_step;

            fg_errno = FG_INVALID_TIME;                             // Report invalid time
            goto error;
        }

        fgSetMinMax(ref[i], &meta);
    }

    // Complete meta data

    meta.time.start      = time[0];
    meta.range.final_ref = ref[num_points - 1];

    fgSetMeta(pol_switch_auto, pol_switch_neg, time[num_points - 1], limits, &meta);

    // Check reference function limits if provided

    if(limits != NULL)
    {
        if((fg_errno = fgCheckRef(ref[0], 0.0, 0.0, &meta, error)))
        {
            goto error;
        }

        for(i = 1 ; i < num_points ; i++)
        {
            grad = (ref[i] - ref[i - 1]) / (time[i] - time[i - 1]);

            if((fg_errno = fgCheckRef(ref[i], grad, 0.0, &meta, error)))
            {
                error->index = i;
                goto error;
            }
        }
    }

    // Prepare table parameters

    pars->table.meta         = meta;
    pars->table.num_points   = num_points;
    pars->table.seg_idx      = 0;
    pars->table.prev_seg_idx = 0;

    // Transfer table ref data if armed_ref is supplied

    if(armed_ref == NULL)
    {
        pars->table.ref = ref;
    }
    else
    {
        pars->table.ref = armed_ref;
        memcpy(armed_ref, ref, num_points * sizeof(ref[0]));
    }

    // Transfer table time data if armed_time is supplied

    if(armed_time == NULL)
    {
        pars->table.time = time;
    }
    else
    {
        pars->table.time = armed_time;
        memcpy(armed_time, time, num_points * sizeof(time[0]));
    }

    return(FG_OK);

    // Error - store error code in meta and return to caller

    error:

        error->fg_errno = fg_errno;
        return(fg_errno);
}



enum FG_func_status fgTableRT(union FG_pars *pars, FG_float func_time, FG_float *ref)
{
    // Pre-function coast

    if(func_time < pars->meta.time.start)
    {
         *ref = pars->meta.range.initial_ref;

         return(FG_GEN_PRE_FUNC);
    }

    // Post-function coast

    if(func_time >= pars->meta.time.end)
    {
         *ref = pars->meta.range.final_ref;

         return(FG_GEN_POST_FUNC);
    }

    // Scan forward through table to find segment containing the current time

    while(func_time >= pars->table.time[pars->table.seg_idx])
    {
        pars->table.seg_idx++;
    }

    // If time before start of segment than back up through the table

    while(func_time < pars->table.time[pars->table.seg_idx - 1])
    {
        pars->table.seg_idx--;
    }

    // If time is in a new segment, calculate the gradient

    if(pars->table.seg_idx != pars->table.prev_seg_idx)
    {
        pars->table.prev_seg_idx = pars->table.seg_idx;
        pars->table.seg_grad     = (pars->table.ref [pars->table.seg_idx] - pars->table.ref [pars->table.seg_idx - 1]) /
                             (pars->table.time[pars->table.seg_idx] - pars->table.time[pars->table.seg_idx - 1]);
    }

    // Calculate reference using segment gradient

    *ref = pars->table.ref[pars->table.seg_idx]  - (pars->table.time[pars->table.seg_idx] - func_time) * pars->table.seg_grad;

    return(FG_GEN_DURING_FUNC);
}

// EOF
