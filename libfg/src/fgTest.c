/*!
 * @file  fgTest.c
 * @brief Generate test functions (STEPS, SQUARE, SINE or COSINE)
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



enum FG_errno fgTestInit(struct FG_limits *limits,
                         bool              pol_switch_auto,
                         bool              pol_switch_neg,
                         enum FG_test_type type,
                         FG_float          initial_ref,
                         FG_float          amplitude_pp,
                         FG_float          num_cycles,
                         FG_float          period,
                         bool              window_enabled,
                         bool              exp_decay_enabled,
                         union  FG_pars   *pars,
                         struct FG_error  *error)
{
    enum   FG_errno fg_errno;       // Error number
    struct FG_error local_error;    // Local error data in case user error is NULL
    struct FG_test  p;              // Local TEST pars - copied to user *pars only if there are no errors
    FG_float        duration;       // Function duration
    FG_float        window[2];      // Max/min window scaling for sine or cosine

    // Reset meta & error structures - uses local_error if error is NULL

    error = fgResetMeta(initial_ref, &p.meta, &local_error, error);

    // Check if number of cycles is less than 1

    if(num_cycles < 0.6)
    {
        error->index   = 1;
        error->data[0] = num_cycles;

        fg_errno = FG_INVALID_TIME;
        goto error;
    }

    // Check if period is negative

    if(period <= 0.0)
    {
        error->index   = 1;
        error->data[0] = period;

        fg_errno = FG_BAD_PARAMETER;
        goto error;
    }

    // Prepare parameter structure

    p.num_cycles     = (uint32_t)(num_cycles + 0.4999);
    p.half_period    = 0.5 * period;
    p.frequency      = 1.0 / period;
    p.amplitude      = amplitude_pp;
    p.type           = type;
    p.window_enabled = false;
    p.exp_decay      = 0.0;
    duration         = (FG_float)p.num_cycles * period;

    // Check if total duration is too long

    if(duration > 1.0E6)
    {
        error->index   = 2;
        error->data[0] = duration;
        error->data[1] = 1.0E6;

        fg_errno = FG_INVALID_TIME;
        goto error;
    }

    // Calculate amplitude related parameters

    fg_errno = FG_OK;

    switch(type)
    {
        case FG_TEST_STEPS:

            p.meta.range.final_ref = p.meta.range.initial_ref + p.amplitude;
            p.amplitude /= (FG_float)p.num_cycles;

            fgSetMinMax(p.meta.range.final_ref, &p.meta);
            break;

        case FG_TEST_SQUARE:
 
            // Square wave is created from 2 x half cycles

            p.num_cycles *= 2;

            fgSetMinMax(initial_ref + p.amplitude, &p.meta);
            break;

        case FG_TEST_SINE:
        case FG_TEST_COSINE:

            p.amplitude *= 0.5;
            window[0]    =  1.0;
            window[1]    = -1.0;

            // Handle the window

            if(window_enabled)
            {
                p.window_enabled = true;

                // Prepare exponential decay factor if enabled

                if(exp_decay_enabled)
                {
                    p.exp_decay = -5.0 / duration;
                }

                // Adjust range scaling if number of cycles is 1
                // This does not take into account the exponential decay because
                // it is too complex to solve analytically for any number of cycles.

                if(p.num_cycles == 1)
                {
                    if(type == FG_TEST_SINE)            // Windowed SINE
                    {
                        window[0] =  0.649519053;            // +3.sqrt(3)/8
                        window[1] = -0.649519053;            // -3.sqrt(3)/8
                    }
                    else                                // Windowed COSINE
                    {
                        window[0] =  1.0 / 8.0;              // +1/8
                        window[1] = -1.0;                    // -1
                    }
                }
            }

            fgSetMinMax(initial_ref + p.amplitude * window[0], &p.meta);
            fgSetMinMax(initial_ref + p.amplitude * window[1], &p.meta);
            break;

        default: // Invalid function type requested

            error->index   = 2;
            error->data[0] = (FG_float)type;

            fg_errno = FG_BAD_PARAMETER;
            goto error;
    }

    // Complete meta data

    fgSetMeta(pol_switch_auto, pol_switch_neg, duration, limits, &p.meta);

    // Copy valid set of parameters to user's pars structure

    memcpy(pars, &p, sizeof(p));

    return(FG_OK);

    // Error - store error code in meta and return to caller

    error:

        error->fg_errno = fg_errno;
        return(fg_errno);
}



enum FG_func_status fgTestRT(union FG_pars *pars, FG_float func_time, FG_float *ref)
{
    uint32_t    period_idx;
    FG_float    radians;
    FG_float    cos_rads  = 0.0;
    FG_float    exp_decay = 1.0;
    FG_float    delta_ref;

    // Pre-function - test functions always start at time 0.0

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

    // Function

    switch(pars->test.type)
    {
        case FG_TEST_STEPS:

            // Calculate period index and clip to number of cycles in case of floating point errors

            period_idx = 1 + (uint32_t)(func_time * pars->test.frequency);

            if(period_idx > pars->test.num_cycles)
            {
                period_idx = pars->test.num_cycles;
            }

            *ref = pars->test.meta.range.initial_ref + pars->test.amplitude * (FG_float)period_idx;

            return(FG_GEN_DURING_FUNC);

        case FG_TEST_SQUARE:

            // Calculate period index and clip to number of cycles in case of floating point errors

            period_idx = 1 + (uint32_t)(2.0 * func_time * pars->test.frequency);

            if(period_idx > pars->test.num_cycles)
            {
                period_idx = pars->test.num_cycles;
            }

            *ref = pars->test.meta.range.initial_ref + (period_idx & 0x1 ? pars->test.amplitude : 0.0);

            return(FG_GEN_DURING_FUNC);

        case FG_TEST_SINE:

            radians   = (2.0 * 3.1415926535897932) * pars->test.frequency * func_time;
            delta_ref = pars->test.amplitude * sinf(radians);
            break;

        case FG_TEST_COSINE:

            radians   = (2.0 * 3.1415926535897932) * pars->test.frequency * func_time;
            cos_rads  = cosf(radians);
            delta_ref = pars->test.amplitude * cos_rads;
            break;

        default: // Invalid function type requested

            return(FG_GEN_POST_FUNC);
    }

    // For SINE and COSINE: Apply cosine window and exp decay if enabled

    if(pars->test.window_enabled)
    {
        if(pars->test.exp_decay != 0.0)
        {
            // Calc exponential decay of amplitude

            exp_decay = expf(func_time * pars->test.exp_decay);
        }

        if(func_time < pars->test.half_period || pars->test.meta.time.duration - func_time < pars->test.half_period)
        {
            // Calc Cosine window

           delta_ref *= 0.5 * (1 - (pars->test.type == FG_TEST_SINE ? cosf(radians) : cos_rads));
        }
    }

    *ref = pars->test.meta.range.initial_ref + delta_ref * exp_decay;

    return(FG_GEN_DURING_FUNC);
}

// EOF
