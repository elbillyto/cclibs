/*!
 * @file  fgPulse.c
 * @brief Generate linear pulse function
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



enum FG_errno fgPulseInit(struct FG_limits *limits,
                         bool               pol_switch_auto,
                         bool               pol_switch_neg,
                         FG_float           initial_ref,
                         FG_float           acceleration,
                         FG_float           linear_rate,
                         FG_float           duration,
                         union  FG_pars    *pars,
                         struct FG_error   *error)
{
    enum   FG_errno fg_errno;        // Error number
    struct FG_error local_error;     // Local error data in case user error is NULL
    struct FG_pulse p;               // Local PULSE pars - copied to user *pars only if there are no errors

    // Reset meta & error structures - uses local_error if error is NULL

    error = fgResetMeta(initial_ref, &p.meta, &local_error, error);

    // Save parameters

    p.ref_pulse    = initial_ref;
    p.acceleration = acceleration;
    p.linear_rate  = linear_rate;

    // If the pulse plateau is a ramp

    if(linear_rate != 0.0)
    {
        // Acceleration must not be zero

        if(acceleration == 0.0)
        {
            fg_errno = FG_BAD_PARAMETER;
            goto error;
        }

        // Set sign of acceleration to be the same as the sign of the linear_rate

        p.acceleration = linear_rate < 0.0 ? -fabs(acceleration) : fabs(acceleration);

        p.meta.time.start = -linear_rate / p.acceleration;
        p.ref_pulse       = initial_ref;

        // Adjust initial reference to allow for parabolic acceleration to the linear ramp

        initial_ref -= 0.5 * p.acceleration * p.meta.time.start * p.meta.time.start;

        // Complete the meta data

        p.meta.range.initial_ref = initial_ref;
        p.meta.range.min_ref     = initial_ref;
        p.meta.range.max_ref     = initial_ref;
        p.meta.range.final_ref   = p.ref_pulse + linear_rate * duration;
        p.meta.range.final_rate  = linear_rate;

        fgSetMinMax(p.meta.range.final_ref, &p.meta);

    }

    // Prepare function limits

    fgSetMeta(pol_switch_auto, pol_switch_neg, duration, limits, &p.meta);

    // Check limits at the beginning, middle and end if supplied

    if(limits != NULL)
    {
        if((fg_errno = fgCheckRef(p.meta.range.initial_ref, 0.0, acceleration, &p.meta, error)) ||
           (fg_errno = fgCheckRef(p.ref_pulse,              linear_rate,  0.0, &p.meta, error)) ||
           (fg_errno = fgCheckRef(p.meta.range.final_ref,   0.0,          0.0, &p.meta, error)))
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



enum FG_func_status fgPulseRT(union FG_pars *pars, FG_float func_time, FG_float *ref)
{
    // Pre-pulse coast

    if(func_time < pars->meta.time.start)
    {
        *ref = pars->meta.range.initial_ref;

        return(FG_GEN_PRE_FUNC);
    }

    // Parabolic acceleration

    if(func_time < 0.0)
    {
        FG_float seg_time = func_time - pars->meta.time.start;

        *ref = pars->meta.range.initial_ref + 0.5 * pars->pulse.acceleration * seg_time * seg_time;

        return(FG_GEN_DURING_FUNC);
    }

    // Linear ramp - this continues beyound the end of the function

    *ref = pars->pulse.ref_pulse + func_time * pars->pulse.linear_rate;


    if(func_time < pars->meta.time.end)
    {
        return(FG_GEN_DURING_FUNC);
    }

    return(FG_GEN_POST_FUNC);
}

// EOF
