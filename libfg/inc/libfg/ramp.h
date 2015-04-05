/*!
 * @file  libfg/ramp.h
 * @brief Generate fast ramp based on Parabola-Parabola function.
 * 
 * RAMP is a special function within libfg in two way:
 *
 * 1. It can start with a non-zero rate of change (by calling fgRampCalc())
 * 2. It can responds to rate of change limits in the calling application.
 *
 * The first feature allows the RAMP function to take over from an a running function.
 * 
 * The second feature allow a RAMP to go as fast as possible, with limits applied
 * by the calling application, while still arriving smoothly at the final value.
 * Note that the function duration returned in fg_meta::meta will be wrong if the
 * reference is limited at any time during the generation of the function. 
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

#ifndef LIBFG_RAMP_H
#define LIBFG_RAMP_H

#include "libfg.h"

// Constants

#define FG_RAMP_NUM_SEGS        2           //!< Number of segments: P-P = 2

/*!
 * RAMP function parameters
 */
struct FG_ramp
{
    struct FG_meta meta;                    //!< Meta data for the armed function - this must be the first in the struct
    bool        is_ramp_positive;           //!< Positive ramp flag.
    bool        is_pre_ramp;                //!< Pre-ramp flag. True if before point of inflexion of 1st parabola.
    FG_float    acceleration;               //!< Parabolic acceleration.
    FG_float    deceleration;               //!< Parabolic deceleration.
    FG_float    linear_rate;                //!< User linear rate.
    FG_float    linear_rate_limit;          //!< Actual linear rate limit.
    FG_float    ref [FG_RAMP_NUM_SEGS+1];   //!< End of segment references. See also #FG_RAMP_NUM_SEGS.
    FG_float    time[FG_RAMP_NUM_SEGS+1];   //!< End of segment times. See also #FG_RAMP_NUM_SEGS.
    FG_float    prev_ramp_ref;              //!< Function ref from previous iteration
    FG_float    prev_returned_ref;          //!< Returned ref from previous iteration
    FG_float    prev_time;                  //!< Time from previous iteration
    FG_float    time_shift;                 //!< Time shift
};

#ifdef __cplusplus
extern "C" {
#endif

// External functions

/*!
 * Initialise RAMP function and check limits.
 *
 * This function uses fgRampCalc() to prepare the Ramp parameters.
 *
 * @param[in]  limits             Pointer to fgc_limits structure (or NULL if no limits checking required).
 * @param[in]  pol_switch_auto    True if polarity switch can be changed automatically.
 * @param[in]  pol_switch_neg     True if polarity switch is currently in the negative position.
 * @param[in]  start_time         Time of start of ramp.
 * @param[in]  initial_ref        Initial reference value (at start_time).
 * @param[in]  final_ref          Final reference value.
 * @param[in]  acceleration       Acceleration of the 1st parabolic segment. Absolute value is used.
 * @param[in]  linear_rate        Maximum linear rate. Absolute value is used.
 * @param[in]  deceleration       Deceleration of the 2nd parabolic segment. Absolute value is used.
 * @param[out] pars               Pointer to fg_pars union containing ramp parameter struct.
 * @param[out] error              Pointer to error information. Set to NULL if not required.
 *
 * @retval FG_OK on success
 * @retval FG_BAD_PARAMETER if acceleration == 0 or deceleration == 0
 * @retval FG_OUT_OF_LIMITS if reference value exceeds limits
 * @retval FG_OUT_OF_RATE_LIMITS if rate of change of reference exceeds limits
 * @retval FG_OUT_OF_ACCELERATION_LIMITS if acceleration exceeds limits
 */
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
                         struct FG_error  *error);



/*!
 * Initialise RAMP function without checking limits.
 *
 * This function is used by fgRampInit(). It does no limit checking. It is a separate function so 
 * a real-time application can initialise a ramp, for example, to take over from another running function.
 * To allow this, the function accepts an initial rate of change.
 *
 * @param[in]  pol_switch_auto    True if polarity switch can be changed automatically.
 * @param[in]  pol_switch_neg     True if polarity switch is currently in the negative position.
 * @param[in]  start_time         Time of the start of the function.
 * @param[in]  initial_rate       Initial rate of change.
 * @param[in]  initial_ref        Initial reference value.
 * @param[in]  final_ref          Final reference value.
 * @param[in]  acceleration       Acceleration of the 1st parabolic segment. Absolute value is used.
 * @param[in]  linear_rate        Maximum linear rate. Absolute value is used.
 * @param[in]  deceleration       Deceleration of the 2nd parabolic segment. Absolute value is used.
 * @param[out] pars               Pointer to ramp function parameters. 
 *                                The coordinates of the transition points between
 *                                the segments of the ramp function are:
 *                                <ul>
 *                                <li>pars->time[0], pars->ref[0]: Max/min of first (accelerating) parabola</li>
 *                                <li>pars->time[1], pars->ref[1]: Connection between accelerating and decelerating parabolas</li>
 *                                <li>pars->time[2], pars->ref[2]: End of the second (decelerating) parabola, also end of the ramp function</li>
 *                                </ul>
 * @param[out] meta               Pointer to diagnostic information structure. Set to NULL if not required.
 */
void fgRampCalc(bool             pol_switch_auto,
                bool             pol_switch_neg,
                FG_float         start_time,
                FG_float         initial_rate,
                FG_float         initial_ref,
                FG_float         final_ref,
                FG_float         acceleration,
                FG_float         linear_rate,
                FG_float         deceleration,
                struct FG_ramp  *pars);



/*!
 * Real-time function to generate a RAMP reference.
 *
 * Derive the reference for the previously-initialised Ramp function at the given time.
 *
 * <strong>NOTE</strong>: Unlike the other libfg functions, TIME MUST NOT GO BACKWARDS
 * because *ref is both an input and an output.
 *
 * @param[in]     pars           Pointer to fg_pars union containing ramp parameter struct.
 * @param[in]     func_time      Time within the function.
 * @param[in,out] ref            Pointer to the reference value. If the application needed to clip
 *                               the reference, fgRampRT() will take this into account.
 *
 * @retval FG_GEN_PRE_FUNC      if time is before the start of the function.
 * @retval FG_GEN_DURING_FUNC   if time is during the function.
 * @retval FG_GEN_POST_FUNC     if time is after the end of the function.
 */
enum FG_func_status fgRampRT(union FG_pars *pars, FG_float func_time, FG_float *ref);

#ifdef __cplusplus
}
#endif

#endif

// EOF
