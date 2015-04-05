/*!
 * @file    libfg/pulse.h
 * @brief   Generate linear pulse functions
 *
 * A pulse is simply a linear section starting at time 0 for the
 * specified duration. If the linear section is a ramp, then the function
 * will generation a parabolic sectioning before time 0 in order to
 * accelerate to the linear rate of the ramp.
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
struct FG_pulse
{
    struct FG_meta meta;                //!< Meta data for the armed function - this must be the first in the struct
    FG_float       ref_pulse;           //!< Reference at start of linear pulse
    FG_float       acceleration;        //!< Acceleration to reach linear_rate
    FG_float       linear_rate;         //!< Pulse linear_rate.
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
 * @param[in]  initial_ref         Initial reference value.
 * @param[in]  acceleration        Acceleration to arrive at linear rate.
 * @param[in]  linear_rate         Linear rate during plateau.
 * @param[in]  duration            Pulse plateau duration.
 * @param[out] pars                Pointer to fg_pars union containing pulse parameter struct.
 * @param[out] error               Pointer to error information. Set to NULL if not required.
 *
 * @retval FG_OK on success
 * @retval FG_BAD_PARAMETER on parameter errors
 * @retval FG_OUT_OF_LIMITS if reference value exceeds limits
 * @retval FG_OUT_OF_RATE_LIMITS if rate of change of reference exceeds limits
 */
enum FG_errno fgPulseInit(struct FG_limits *limits,
                         bool               pol_switch_auto,
                         bool               pol_switch_neg,
                         FG_float           initial_ref,
                         FG_float           acceleration,
                         FG_float           linear_rate,
                         FG_float           duration,
                         union  FG_pars    *pars,
                         struct FG_error   *error);



/*!
 * Real-time function to generate a PULSE reference.
 *
 * @param[in]  pars             Pointer to fg_pars union containing pulse parameter struct.
 * @param[in]  func_time        Time within the function.
 * @param[out] ref              Pointer to reference value.
 *
 * @retval FG_GEN_PRE_FUNC      if time is before the start of the function.
 * @retval FG_GEN_DURING_FUNC   if time is during the function.
 * @retval FG_GEN_POST_FUNC     if time is after the end of the function.
 */
enum FG_func_status fgPulseRT(union FG_pars *pars, FG_float time, FG_float *ref);

#ifdef __cplusplus
}
#endif

#endif

// EOF
