/*!
 * @file  fg.c
 * @brief Function Generation library top-level source file.
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

#include "libfg.h"



struct FG_error *fgResetMeta(FG_float         initial_ref,
                             struct FG_meta  *meta,
                             struct FG_error *local_error,
                             struct FG_error *error)
{
    // Reset all field in meta structure

    meta->polarity            = FG_FUNC_POL_ZERO;
    meta->limits_inverted     = false;

    meta->time.start          = 0.0;
    meta->time.end            = 0.0;
    meta->time.duration       = 0.0;

    meta->range.initial_ref   = initial_ref;
    meta->range.final_ref     = initial_ref;
    meta->range.min_ref       = initial_ref;
    meta->range.max_ref       = initial_ref;
    meta->range.final_rate    = 0.0;

    meta->limits.min          = 0.0;
    meta->limits.max          = 0.0;
    meta->limits.rate         = 0.0;
    meta->limits.acceleration = 0.0;

    // Use local_error if error is NULL

    if(error == NULL)
    {
        error = local_error;
    }

    // Reset all field in error structure if error pointer is valid

    if(error != NULL)
    {
        uint32_t idx;

        error->index    = 0;
        error->fg_errno = FG_OK;

        // Explicitly set floats to zero because early TI DSPs do not use IEEE758
        // floating point format. For example, for the TMS320C32, 0.0 is 0x80000000 
        // while 0x00000000 has the value 1.0.

        for(idx = 0 ; idx < FG_ERR_DATA_LEN ; idx++)
        {
            error->data[idx] = 0.0;
        }
    }

    return(error);
}



void fgSetMinMax(FG_float ref, struct FG_meta *meta)
{
    if(ref > meta->range.max_ref)
    {
        meta->range.max_ref = ref;
    }
    else if(ref < meta->range.min_ref)
    {
        meta->range.min_ref = ref;
    }
}



void fgSetMeta(bool              pol_switch_auto,
               bool              pol_switch_neg,
               FG_float          end_time,
               struct FG_limits *limits,
               struct FG_meta   *meta)
{
    // Calculate function duration from start and end times

    meta->time.end      = end_time;
    meta->time.duration = end_time - meta->time.start;

    // Set meta polarity flag

    if(meta->range.max_ref > 0.0)
    {
        meta->polarity |= FG_FUNC_POL_POSITIVE;
    }

    if(meta->range.min_ref < 0.0)
    {
        meta->polarity |= FG_FUNC_POL_NEGATIVE;
    }

    // Set limits inversion control based on the switch control and state

    meta->limits_inverted = (pol_switch_auto == false && pol_switch_neg == true) ||
                            (pol_switch_auto == true  && meta->polarity == FG_FUNC_POL_NEGATIVE);

    // Initialise limits if provided, adjusting by a small clip limit factor to avoid rounding errors

    if(limits != NULL)
    {
        // Set min/max limits based on limits inversion control

        if(meta->limits_inverted)
        {
            // Invert limits - only ever required for unipolar converters so limits->neg will be zero

            meta->limits.max = -(1.0 - FG_CLIP_LIMIT_FACTOR) * limits->min;
            meta->limits.min = -(1.0 + FG_CLIP_LIMIT_FACTOR) * limits->pos;
        }
        else // Limits do not need to be inverted
        {
            meta->limits.max = (1.0 + FG_CLIP_LIMIT_FACTOR) * limits->pos;
            meta->limits.min = (limits->neg < 0.0 ? (1.0 + FG_CLIP_LIMIT_FACTOR) * limits->neg :
                                                    (1.0 - FG_CLIP_LIMIT_FACTOR) * limits->min);
        }

        // Set rate/acceleration limits

        meta->limits.rate         = (1.0 + FG_CLIP_LIMIT_FACTOR) * limits->rate;
        meta->limits.acceleration = (1.0 + FG_CLIP_LIMIT_FACTOR) * limits->acceleration;
    }
}



enum FG_errno fgCheckRef(FG_float         ref,
                         FG_float         rate,
                         FG_float         acceleration,
                         struct FG_meta  *meta,
                         struct FG_error *error)
{
    // Check reference against min/max limits

    if(ref > meta->limits.max || ref < meta->limits.min)
    {
        error->data[0] = ref;
        error->data[1] = meta->limits.min;
        error->data[2] = meta->limits.max;

        return(FG_OUT_OF_LIMITS);
    }

    // Check rate of change of reference if limit is positive

    if(meta->limits.rate > 0.0 && fabs(rate) > meta->limits.rate)
    {
        error->data[0] = rate;
        error->data[1] = meta->limits.rate;

        return(FG_OUT_OF_RATE_LIMITS);
    }

    // Check acceleration if limit is positive

    if(meta->limits.acceleration > 0.0 && fabs(acceleration) > meta->limits.acceleration)
    {
        error->data[0] = acceleration;
        error->data[1] = meta->limits.acceleration;

        return(FG_OUT_OF_ACCELERATION_LIMITS);
    }

    return(FG_OK);
}

// EOF
