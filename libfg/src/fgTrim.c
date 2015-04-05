/*!
 * @file  fgTrim.c
 * @brief Generate linear and cubic trim functions
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



enum FG_errno fgTrimInit(struct FG_limits *limits,
                         bool              pol_switch_auto,
                         bool              pol_switch_neg,
                         enum              FG_trim_type type,
                         FG_float          initial_ref,
                         FG_float          final_ref,
                         FG_float          duration,
                         union  FG_pars   *pars,
                         struct FG_error  *error)
{
    enum   FG_errno fg_errno;         // Error number
    struct FG_error local_error;      // Local error data in case user error is NULL
    struct FG_trim  p;                // Local TRIM pars - copied to user *pars only if there are no errors
    bool            is_trim_inverted; // Inverted trim flag
    FG_float        acceleration;     // Acceleration
    FG_float        duration1;        // Trim duration
    FG_float        duration2;        // Trim duration based on acceleration limit
    FG_float        delta_ref;        // Trim reference change
    FG_float        rate_lim;         // Limiting

    // Reset meta & error structures - uses local_error if error is NULL

    error = fgResetMeta(initial_ref, &p.meta, &local_error, error);

    // Assess if ramp is rising or falling

    delta_ref = final_ref - initial_ref;

    if(delta_ref < 0.0)
    {
        is_trim_inverted     = true;
        p.meta.range.min_ref = final_ref;
        p.meta.range.max_ref = initial_ref;
    }
    else
    {
        is_trim_inverted     = false;
        p.meta.range.min_ref = initial_ref;
        p.meta.range.max_ref = final_ref;
    }

    // Prepare cubic factors according to trim type

    switch(type)
    {
        case FG_TRIM_CUBIC:

            p.a = 1.0;
            p.c = 1.5;
            break;

        case FG_TRIM_LINEAR:

            p.a = 0.0;
            p.c = 1.0;
            break;

        default:

            error->index   = 1;
            error->data[0] = type;

            fg_errno = FG_BAD_PARAMETER;
            goto error;
    }

    // Calculate or check duration and complete cubic factors

    if(duration < 1.0E-6)                
    {
        // If duration is zero then limits must be supplied

        if(limits == NULL || limits->rate == 0.0)              
        {
            error->index = 2;

            fg_errno = FG_BAD_PARAMETER;
            goto error;
        }

        // Calculate rate of change limit

        rate_lim = limits->rate;

        if(is_trim_inverted)
        {
            rate_lim = -rate_lim;
        }

        // Calculate duration based on rate_lim

        duration1 = p.c * delta_ref / rate_lim;

        // If Cubic trim

        if(p.a != 0.0)
        {
            // Protect against zero acceleration

            if(limits->acceleration <= 1.0E-6)
            {
                error->index   = 3;
                error->data[0] = limits->acceleration;

                fg_errno = FG_BAD_PARAMETER;
                goto error;
            }

            // Calculate duration based on acceleration limit

            duration2 = sqrt(fabs(6.0 * delta_ref / limits->acceleration));     
                                                                               
            if(duration1 < duration2)                                          
            {
                duration1 = duration2;
            }
        }

        duration = duration1;
    }

    p.a *= -2.0 * delta_ref / (duration * duration * duration);
    p.c *= delta_ref / duration;

    // Calculate offsets

    p.time_offset = 0.5 * duration;
    p.ref_offset  = 0.5 * (initial_ref + final_ref);

    // Calculate acceleration  (note a=0 for Linear trim)

    acceleration = fabs(3.0 * p.a * duration);

    // Complete meta data

    p.meta.range.final_ref = final_ref;

    fgSetMeta(pol_switch_auto, pol_switch_neg, duration, limits, &p.meta);

    // Check limits at the beginning, middle and end if supplied

    if(limits != NULL)
    {
        if((fg_errno = fgCheckRef(initial_ref,  0.0, type == FG_TRIM_CUBIC ? acceleration : 0.0, &p.meta, error)) ||
           (fg_errno = fgCheckRef(p.ref_offset, p.c, 0.0, &p.meta, error)) ||
           (fg_errno = fgCheckRef(final_ref,    0.0, 0.0, &p.meta, error)))
        {
            goto error;
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



enum FG_func_status fgTrimRT(union FG_pars *pars, FG_float func_time, FG_float *ref)
{
    FG_float    seg_time;

    // Pre-function - trim functions always start at time 0.0

    if(func_time < 0.0)
    {
        *ref = pars->meta.range.initial_ref;

        return(FG_GEN_PRE_FUNC);
    }

    // Post-function

    if(func_time >= pars->meta.time.end)
    {
        *ref = pars->meta.range.final_ref;

        return(FG_GEN_POST_FUNC);
    }

    // Function - linear or cubic segment

    seg_time = func_time - pars->trim.time_offset;
    *ref     = pars->trim.ref_offset + seg_time * (pars->trim.a * seg_time * seg_time + pars->trim.c);

    return(FG_GEN_DURING_FUNC);
}

// EOF
