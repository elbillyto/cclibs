/*!
 * @file  fgRamp.c
 * @brief Generate fast ramp based on Parabola-Parabola function
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



enum FG_errno fgRampInit(struct FG_limits *limits, 
                         bool              pol_switch_auto,
                         bool              pol_switch_neg,
                         FG_float          start_time,
                         FG_float          initial_ref,
                         FG_float          final_ref,
                         FG_float          acceleration,
                         FG_float          linear_rate,
                         FG_float          deceleration,
                         union  FG_pars   *pars,
                         struct FG_error  *error)
{
    enum   FG_errno fg_errno;                    // Error number
    struct FG_error local_error;                 // Local error data in case user error is NULL
    struct FG_ramp  p;                            // Local RAMP pars - copied to user *pars if there are no errors

    // Reset meta & error structures - uses local_error if error is NULL

    error = fgResetMeta(initial_ref, &p.meta, &local_error, error);

    // Check that parameters are valid

    if(acceleration == 0.0 || deceleration == 0.0)
    {
        // Reset meta structure - uses local_meta if meta is NULL

        fg_errno = FG_BAD_PARAMETER;
        goto error;
    }

    // Calculate ramp parameters always with zero initial ramp rate

    fgRampCalc(pol_switch_auto,
                pol_switch_neg,
                start_time,
                0.0,            // initial_rate is always zero when armed by the user
                initial_ref,
                final_ref,
                acceleration,
                linear_rate,
                deceleration,
                &p);

    // Check limits if supplied

    if(limits != NULL)
    {
        // Prepare function limits in meta structure

        fgSetMeta(pol_switch_auto, pol_switch_neg, p.meta.time.end, limits, &p.meta);

        // Check limits at the start of the parabolic acceleration (segment 1)

        if((fg_errno = fgCheckRef(initial_ref, 0.0, p.acceleration, &p.meta, error)))
        {
            error->index = 1;
            goto error;
        }

        // Check limits at the end of the parabolic deceleration (segment 2)

        if((fg_errno = fgCheckRef(final_ref, 0.0, p.deceleration, &p.meta, error)))
        {
            error->index = 2;
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



void fgRampCalc(bool            pol_switch_auto,
                bool            pol_switch_neg,
                FG_float        start_time,
                FG_float        initial_rate,
                FG_float        initial_ref,
                FG_float        final_ref,
                FG_float        acceleration,
                FG_float        linear_rate,
                FG_float        deceleration,
                struct FG_ramp *pars)
{
    FG_float    delta_ref;              // Initial ref minus final ref
    FG_float    ref0;                   // Ref at t=0 for first parabola
    FG_float    overshoot_rate_limit;   // Limiting initial rate of change before overshoot occurs
    FG_float    seg_ratio;              // Ratio between the two segments

    // Reset meta structure but without the error structure as there is no error checking in fgRampCalc()

    fgResetMeta(initial_ref, &pars->meta, NULL, NULL);

    // Prepare variables assuming ascending (positive) ramp

    pars->prev_time         =  start_time;
    pars->is_ramp_positive  =  true;
    pars->acceleration      =  fabs(acceleration);
    pars->deceleration      = -fabs(deceleration);
    pars->linear_rate       =  fabs(linear_rate);
    pars->linear_rate_limit =  fabs(initial_rate);
    pars->prev_ramp_ref     =  pars->prev_returned_ref = initial_ref;
    delta_ref               =  final_ref - initial_ref;
    overshoot_rate_limit    =  sqrt(-2.0 * pars->deceleration * fabs(delta_ref));

    // Set up accelerations according to ramp direction and possible overshoot

    if((delta_ref >= 0.0 && initial_rate >   overshoot_rate_limit) ||
       (delta_ref <  0.0 && initial_rate >= -overshoot_rate_limit))
    {
        // Descending (negative) ramp
        
            pars->is_ramp_positive =  false;
            pars->acceleration  = -pars->acceleration;
            pars->deceleration  = -pars->deceleration;
    }

    // Set time_shift and ref0 and delta_ref to take into account the initial rate of change

    pars->time_shift  = -initial_rate / pars->acceleration;
    pars->is_pre_ramp = (pars->time_shift > 0.0);

    ref0      = initial_ref + 0.5 * initial_rate * pars->time_shift;
    delta_ref = final_ref - ref0;

    // Calculate ramp parameters

    seg_ratio = pars->deceleration / (pars->deceleration - pars->acceleration);

    pars->time[0] = 0.0;
    pars->time[2] = sqrt(2.0 * delta_ref / (seg_ratio * pars->acceleration));
    pars->time[1] = pars->time[2] * seg_ratio;

    pars->ref[0]  = ref0;
    pars->ref[1]  = ref0 + delta_ref * seg_ratio;
    pars->ref[2]  = final_ref;

    // Set min/max

    fgSetMinMax(initial_ref, &pars->meta);
    fgSetMinMax(final_ref,   &pars->meta);

    // If time_shift is positive then include point of inflexion of first parabola in min/max check

    if(pars->time_shift > 0.0)
    {
        fgSetMinMax(ref0, &pars->meta);
    }

    // Complete meta data without limits

    pars->meta.time.start      =  start_time;
    pars->meta.range.final_ref = final_ref;

    fgSetMeta(pol_switch_auto,
              pol_switch_neg,
              start_time + pars->time[2] + pars->time_shift,        // end of function time
              NULL,                                                 // No limits
              &pars->meta);
}



enum FG_func_status fgRampRT(union FG_pars *pars, FG_float func_time, FG_float *ref)
{
    enum FG_func_status status = FG_GEN_DURING_FUNC; // Set default return status
    uint32_t    time_shift_alg  = 0;                // Time shift adjustment algorithm index
    FG_float    r;
    FG_float    ref_rate_limit;                     // Limit on ref due to rate limit
    FG_float    period;                             // Time period calculated using prev_time
    FG_float    ref_time;                           // Time within the segment in seconds

    // Pre-function coast

    if(func_time < pars->meta.time.start)
    {
        r = pars->meta.range.initial_ref;

        status = FG_GEN_PRE_FUNC;
    }
    else
    {
        // If reference received from previous iteration was changed, and isn't blocked

        if(*ref != pars->ramp.prev_ramp_ref && *ref != pars->ramp.prev_returned_ref)
        {
            // Identify time shift adjustment algorithm according to ramp direction

            if(pars->ramp.is_ramp_positive)
            {
                // Positive (rising) ramp 

                if(*ref > pars->ramp.ref[0])
                {
                    if(pars->ramp.is_pre_ramp)
                    {
                         time_shift_alg = 1;
                    }
                    else if(*ref <= pars->ramp.ref[1])
                    {
                         time_shift_alg = 2;
                    }
                    else if(*ref <= pars->ramp.ref[2])
                    {
                         time_shift_alg = 3;
                    }
                }
            }
            else // Negative (falling) ramp
            {
                if(*ref < pars->ramp.ref[0])
                {
                    if(pars->ramp.is_pre_ramp)
                    {
                         time_shift_alg = 1;
                    }
                    else if(*ref >= pars->ramp.ref[1])
                    {
                         time_shift_alg = 2;
                    }
                    else if(*ref >= pars->ramp.ref[2])
                    {
                         time_shift_alg = 3;
                    }
                }
            }

            // Adjust time shift using appropriate algorithm

            switch(time_shift_alg)
            {
                case 1: pars->ramp.time_shift = pars->ramp.prev_time - pars->ramp.meta.time.start +
                                           sqrt(2.0 * (*ref - pars->ramp.ref[0]) / pars->ramp.acceleration);
                        break;

                case 2: pars->ramp.time_shift = pars->ramp.prev_time - pars->ramp.meta.time.start -
                                           sqrt(2.0 * (*ref - pars->ramp.ref[0]) / pars->ramp.acceleration);
                        break;

                case 3: pars->ramp.time_shift = pars->ramp.prev_time - pars->ramp.meta.time.start -
                                           (pars->ramp.time[2] - sqrt(2.0 * (*ref - pars->ramp.ref[2]) / pars->ramp.deceleration)); // deceleration always +ve
                        break;

                default:break;
            }
        }

        // Calculate new ref_time including delay and time_shift

        ref_time = func_time - pars->ramp.meta.time.start - pars->ramp.time_shift;

        // Parabolic acceleration

        if(ref_time <= pars->ramp.time[1])
        {
            r = pars->ramp.ref[0] + 0.5 * pars->ramp.acceleration * ref_time * ref_time;

            // Reset is_pre_ramp once the main part of the ramp is started

            if(ref_time >= 0.0)
            {
                pars->ramp.is_pre_ramp = false;
            }
        }

        // Parabolic deceleration

        else if(ref_time <= pars->ramp.time[2])
        {
            ref_time -= pars->ramp.time[2];        // ref_time is relative to end of parabola (negative)
            r = pars->ramp.ref[2] + 0.5 * pars->ramp.deceleration * ref_time * ref_time;
        }

        // Coast

        else
        {
            r = pars->ramp.ref[2];

            // End of function
 
            status = FG_GEN_POST_FUNC;
        }

        // Keep ramp reference for next iteration (before rate limiter)

        pars->ramp.prev_ramp_ref = r;

        // Apply rate limit if active

        period = func_time - pars->ramp.prev_time;

        if(pars->ramp.linear_rate > 0.0 && period > 0.0)
        {
            if(pars->ramp.linear_rate_limit < pars->ramp.linear_rate)
            {
                pars->ramp.linear_rate_limit = pars->ramp.linear_rate;
            }

            if(r > *ref)
            {
                // Positive rate of change

                ref_rate_limit = *ref + pars->ramp.linear_rate_limit * period;

                if(r > ref_rate_limit)
                {
                    r = ref_rate_limit;
                }
            }
            else if(r < *ref)
            {
                // Negative rate of change

                ref_rate_limit = *ref - pars->ramp.linear_rate_limit * period;

                if(r < ref_rate_limit)
                {
                    r = ref_rate_limit;
                }
            }

            // Adjust linear rate limit if greater than user value, respecting acceleration

            if(pars->ramp.linear_rate_limit > pars->ramp.linear_rate)
            {
                pars->ramp.linear_rate_limit -= period * fabs(ref_time <= pars->ramp.time[1] ? pars->ramp.acceleration : pars->ramp.deceleration);
            }
        }
    }

    // Keep returned reference and time for next iteration

    pars->ramp.prev_returned_ref = *ref;
    pars->ramp.prev_time         = func_time;

    // Return new reference after rate limit

    *ref = r;

    return(status);
}

// EOF
