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
#include "libfg/pulse.h"



enum fg_error fgPulseInit(struct fg_limits *limits,
                         bool   pol_switch_auto,
                         bool   pol_switch_neg,
                         double delay, 
                         float  initial_ref,
                         float  linear_rate,
                         float  duration,
                         struct fg_pulse *pars,
                         struct fg_meta *meta)
{
    enum fg_error  fg_error;         // Error code
    struct fg_meta local_meta;       // Local meta data in case user meta is NULL
    struct fg_pulse p;               // Local PULSE pars - copied to user *pars only if there are no errors

    // Reset meta structure - uses local_meta if meta is NULL

    meta = fgResetMeta(meta, &local_meta, delay, initial_ref);

    // Save parameters

    p.delay       = delay;
    p.duration    = duration;
    p.initial_ref = initial_ref;
    p.linear_rate = linear_rate;
    p.final_ref   = initial_ref + linear_rate * duration;

    // Complete meta data

    fgSetMinMax(meta, p.initial_ref);
    fgSetMinMax(meta, p.final_ref);

    meta->duration         = duration;
    meta->range.end        = p.final_ref;
    meta->range.final_rate = linear_rate;

    fgSetFuncPolarity(meta, pol_switch_auto, pol_switch_neg);

    // Check limits at the beginning, middle and end if supplied

    if(limits != NULL)
    {
        if((fg_error = fgCheckRef(limits, p.initial_ref, linear_rate, 0.0, meta)) ||
           (fg_error = fgCheckRef(limits, p.final_ref,   linear_rate, 0.0, meta)))
        {
            goto error;
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



enum fg_gen_status fgPulseGen(struct fg_pulse *pars, const double *time, float *ref)
{
    double   func_time;                     // Time within function

    // Both *time and delay must be 64-bit doubles if time is UNIX time

    func_time = *time - pars->delay;

    // Pre-pulse coast

    if(func_time < 0.0)
    {
        *ref = pars->initial_ref;

        return(FG_GEN_PRE_FUNC);
    }

    // Pulse

    else if(func_time <= pars->duration)
    {
        *ref   = pars->initial_ref + func_time * pars->linear_rate;

        return(FG_GEN_DURING_FUNC);
    }

    // Post-pulse coast

    *ref = pars->final_ref;

    return(FG_GEN_POST_FUNC);
}

// EOF
