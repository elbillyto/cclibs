/*!
 * @file  libfg/plep.h
 * @brief Generate Parabola-Linear-Exponential-Parabola (PLEP) functions
 *
 * A PLEP can have up to five segments: Parabola-Linear-Exponential-Parabola-Parabola.
 * The exponential is only interesting when ramping down a 1-quadrant converter. The
 * normalised PLEP is always calculated as a descending function: if the PLEP is
 * ascending, the normalised PLEP is obtained by reflecting it around zero.
 * The function is defined by the arrays fg_plep::ref and fg_plep::time. 
 * These contain the segment times and normalised values.
 *
 * The PLEP function is special because the final reference can have a non-zero rate
 * of change. If the final rate of change is not zero, this adds a fifth parabolic
 * segment. This can be an extension of the fourth parabola, or it can have the
 * opposite acceleration.
 *
 * For further details, see <a href="../pdf/PLEP.pdf">PLEP.pdf</a>.
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

#ifndef LIBFG_PLEP_H
#define LIBFG_PLEP_H

#include "libfg.h"

// Constants

#define FG_PLEP_NUM_SEGS        5                   //!< Number of segments: P-L-E-P-P = 5

/*!
 * PLEP function parameters
 */
struct FG_plep
{
    struct FG_meta meta;                            //!< Meta data for the armed function - this must be the first in the struct
    FG_float       normalisation;                   //!< 1.0 for descending PLEP, -1.0 for ascending PLEP.
    FG_float       acceleration;                    //!< Parabolic acceleration/deceleration.
    FG_float       final_acc;                       //!< Normalised final parabolic acceleration.
    FG_float       linear_rate;                     //!< Linear rate of change (always negative).
    FG_float       final_rate;                      //!< Normalised final linear rate of change.
    FG_float       ref_exp;                         //!< Initial reference for exponential segment.
    FG_float       inv_exp_tc;                      //!< Time constant for exponential segment.
    FG_float       exp_final;                       //!< End reference of exponential segment.
    FG_float       seg_ref [FG_PLEP_NUM_SEGS+1];    //!< Start/End of segment normalised references. See also #FG_PLEP_NUM_SEGS.
    FG_float       seg_time[FG_PLEP_NUM_SEGS+1];    //!< Start/End of segment times. See also #FG_PLEP_NUM_SEGS.
};

#ifdef __cplusplus
extern "C" {
#endif

// External functions

/*!
 * Initialise PLEP function.
 *
 * @param[in]  limits             Pointer to fgc_limits structure (or NULL if no limits checking required).
 * @param[in]  pol_switch_auto    True if polarity switch can be changed automatically.
 * @param[in]  pol_switch_neg     True if polarity switch is currently in the negative position.
 * @param[in]  initial_ref        Initial reference value.
 * @param[in]  final_ref          Final reference value.
 * @param[in]  final_rate         Final rate of change.
 * @param[in]  acceleration       Acceleration/deceleration value. Absolute value is used.
 * @param[in]  linear_rate        Maximum linear rate. Absolute value is used.
 * @param[in]  exp_tc             Time constant for exponential section.
 * @param[in]  exp_final          Final reference for exponential section.
 * @param[out] pars               Pointer to fg_pars union containing plep parameter struct.
 * @param[out] error              Pointer to error information. Set to NULL if not required.
 *
 * @retval FG_OK on success
 * @retval FG_BAD_PARAMETER if any input parameters are invalid
 * @retval FG_OUT_OF_LIMITS if reference value exceeds limits
 * @retval FG_OUT_OF_RATE_LIMITS if rate of change of reference exceeds limits
 * @retval FG_OUT_OF_ACCELERATION_LIMITS if acceleration exceeds limits
 */
enum FG_errno fgPlepInit(struct FG_limits *limits, 
                         bool              pol_switch_auto,
                         bool              pol_switch_neg,
                         FG_float          initial_ref,
                         FG_float          final_ref,
                         FG_float          final_rate,
                         FG_float          acceleration,
                         FG_float          linear_rate,
                         FG_float          exp_tc,
                         FG_float          exp_final,
                         union  FG_pars   *pars,
                         struct FG_error  *error);



/*!
 * Real-time function to generate a PLEP reference.
 *
 * Derive the reference for the previously-initialised PLEP function at the given
 * function time. The PLEP will start at time 0. The PLEP can have a non-zero
 * final rate of change.
 *
 * The coordinates are defined for a normalised, descending, PLEP function.
 * The reference is adjusted (de-normalised) if the PLEP is ascending, by simply
 * flipping the sign.
 *
 * @param[in]  pars             Pointer to fg_pars union containing plep parameters structure.
 *                              This contains the coordinates of the transition points between the
 *                              segments of the PLEP function:
 *                              <ul>
 *                              <li>pars->plep.seg_time[0], pars->plep.seg_ref[0]: Start of the first parabola</li>
 *                              <li>pars->plep.seg_time[1], pars->plep.seg_ref[1]: End of the first parabola</li>
 *                              <li>pars->plep.seg_time[2], pars->plep.seg_ref[2]: End of the linear segment</li>
 *                              <li>pars->plep.seg_time[3], pars->plep.seg_ref[3]: End of the exponential segments</li>
 *                              <li>pars->plep.seg_time[4], pars->plep.seg_ref[4]: End of the second (decelerating) parabola</li>
 *                              <li>pars->plep.seg_time[5], pars->plep.seg_ref[5]: End of the third (accelerating) parabola and end of the PLEP function</li>
 *                              </ul>
 * @param[in]  func_time        Time within the function.
 * @param[out] ref              Pointer to the reference value.
 *
 * @retval FG_GEN_PRE_FUNC      if time is before the start of the function (i.e. if time < 0).
 * @retval FG_GEN_DURING_FUNC   if time is during the function.
 * @retval FG_GEN_POST_FUNC     if time is after the end of the function.
 */
enum FG_func_status fgPlepRT(union FG_pars *pars, FG_float func_time, FG_float *ref);

#ifdef __cplusplus
}
#endif

#endif

// EOF
