/*!
 * @file    pulse.h
 * @brief   Generate linear pulse functions
 *
 * A pulse is simply a linear section at a specified time and for
 * a specified duration. As such, it is very similar to linear trim
 * except that the gradient is specified, instead of the final reference.
 * 
 * <h2>Contact</h2>
 *
 * cclibs-devs@cern.ch
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

#ifndef LIBFG_PULSE_H
#define LIBFG_PULSE_H

#include "libfg.h"

// Types

/*!
 * Pulse function parameters
 */
struct fg_pulse
{
    double              delay;          //!< Time before start of pulse.
    float               duration;       //!< Pulse duration.
    float               initial_ref;    //!< Initial reference.
    float               linear_rate;    //!< Pulse gradient.
    float               final_ref;      //!< Final reference.
};

#ifdef __cplusplus
extern "C" {
#endif

// External functions

/*!
 * Initialise PULSE function.
 *
 * @param[in]  limits              Pointer to fgc_limits structure (or NULL if no limits checking required).
 * @param[in]  pol_switch_auto     Set true if polarity switch can be changed automatically.
 * @param[in]  pol_switch_neg      Set true if polarity switch is currently in the negative position.
 * @param[in]  delay               Delay before the start of the function.
 * @param[in]  initial_ref         Initial reference value.
 * @param[in]  linear_rate         Linear rate.
 * @param[in]  duration            Function duration (Zero to go as fast as limits allow).
 * @param[out] pars                Pointer to pulse function parameters.
 * @param[out] meta                Pointer to diagnostic information. Set to NULL if not required.
 *
 * @retval FG_OK on success
 * @retval FG_BAD_PARAMETER on parameter errors
 * @retval FG_OUT_OF_LIMITS if reference value exceeds limits
 * @retval FG_OUT_OF_RATE_LIMITS if rate of change of reference exceeds limits
 */
enum fg_error fgPulseInit(struct fg_limits *limits,
                         bool   pol_switch_auto,
                         bool   pol_switch_neg,
                         double delay, 
                         float  initial_ref,
                         float  linear_rate,
                         float  duration,
                         struct fg_pulse *pars,
                         struct fg_meta *meta);



/*!
 * Generate the reference for the Pulse function.
 *
 * @param[in]  pars            Pointer to pulse function parameters.
 * @param[in]  time            Pointer to time within the function.
 * @param[out] ref             Pointer to reference value.
 *
 * @retval FG_GEN_PRE_FUNC     if time is before the start of the function.
 * @retval FG_GEN_DURING_FUNC  if time is during the function.
 * @retval FG_GEN_POST_FUNC    if time is after the end of the function.
 */
enum fg_gen_status fgPulseGen (struct fg_pulse *pars, const double *time, float *ref);

#ifdef __cplusplus
}
#endif

#endif

// EOF
