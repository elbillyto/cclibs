/*!
 * @file    libfg/pppl.h
 * @brief   Generate Parabola-Parabola-Parabola-Linear (PPPL) functions
 *
 * The PPPL function allows a series of plateaus to be linked by smooth
 * parabolic accelerations and decelerations.
 *
 * <h2>PPPL Reference Definition</h2>
 *
 * <ul>
 * <li>Seven parameters define each PPPL section from four segments</li>
 * <li>Up to eight PPPL sections can be chained together</li>
 * <li>If ACCELERATION2 is zero, it becomes a PLPL</li>
 * <li>Ramps can be up or down</li>
 * </ul>
 *
 * \image html  PPPL_parameters.png
 * \image latex PPPL_parameters.png "PPPL Reference Definition" width=\textwidth
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
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef LIBFG_PPPL_H
#define LIBFG_PPPL_H

#include "libfg.h"

// Constants

#define FG_MAX_PPPLS      8                              //!< Max number of PPPL sections that can be chained together
#define FG_PPPL_NUM_SEGS  4                              //!< Number of segments in each PPPL section (P-P-P-L = 4)
#define FG_MAX_PPPL_SEGS  FG_PPPL_NUM_SEGS*FG_MAX_PPPLS  //!< Max number of PPPL segments

/*!
 * PPPL function parameters.
 * \f$ref = a_{2} \cdot t^{2} + a_{1} \cdot t + a_{0}\f$,
 * where \f$t\f$ is time in the segment (always negative, since \f$t=0\f$ corresponds to the end of each segment).
 */
struct FG_pppl
{                                                       
    struct FG_meta meta;                            //!< Meta data for the armed function - this must be the first in the struct
    uint32_t       seg_idx;                         //!< Current segment index.
    uint32_t       num_segs;                        //!< Total number of segments (4*number of PPPLs).
    FG_float       seg_time[FG_MAX_PPPL_SEGS];      //!< Times of the end of each segment. See also #FG_PPPL_NUM_SEGS and #FG_MAX_PPPLS.
    FG_float       seg_a0  [FG_MAX_PPPL_SEGS];      //!< Coefficient for constant term.  \f$ref = seg_a_{2} \cdot t^{2} + seg_a_{1} \cdot t + seg_a_{0}\f$
    FG_float       seg_a1  [FG_MAX_PPPL_SEGS];      //!< Coefficient for linear term.    \f$ref = seg_a_{2} \cdot t^{2} + seg_a_{1} \cdot t + seg_a_{0}\f$
    FG_float       seg_a2  [FG_MAX_PPPL_SEGS];      //!< Coefficient for quadratic term. \f$ref = seg_a_{2} \cdot t^{2} + seg_a_{1} \cdot t + seg_a_{0}\f$
};

#ifdef __cplusplus
extern "C" {
#endif

// External functions

/*!
 * Initialise PPPL function.
 *
 * @param[in]  limits                Pointer to fgc_limits structure (or NULL if no limits checking required).
 * @param[in]  pol_switch_auto       True if polarity switch can be changed automatically.
 * @param[in]  pol_switch_neg        True if polarity switch is currently in the negative position.
 * @param[in]  initial_ref           Initial reference value.
 * @param[in]  acceleration1         Accelerations of first (parabolic) segments.
 * @param[in]  acceleration1_num_els Number of elements in acceleration1 array.
 * @param[in]  acceleration2         Accelerations of second (parabolic) segments.
 * @param[in]  acceleration2_num_els Number of elements in acceleration2 array.
 * @param[in]  acceleration3         Accelerations of third  (parabolic) segments.
 * @param[in]  acceleration3_num_els Number of elements in acceleration3 array.
 * @param[in]  rate2                 Rates of change at start of second (parabolic) segments.
 * @param[in]  rate2_num_els         Number of elements in rate2 array.
 * @param[in]  rate4                 Rates of change of fourth (linear) segments.
 * @param[in]  rate4_num_els         Number of elements in rate4 array.
 * @param[in]  ref4                  References at start of fourth (linear) segments.
 * @param[in]  ref4_num_els          Number of elements in ref4 array.
 * @param[in]  duration4             Durations of fourth (linear) segments.
 * @param[in]  duration4_num_els     Number of elements in duration4 array.
 * @param[out] pars                  Pointer to fg_pars union containing pppl parameter struct.
 * @param[out] error                 Pointer to error information. Set to NULL if not required.
 *
 * @retval FG_OK on success
 * @retval FG_BAD_ARRAY_LEN if lengths of the input arrays are different or invalid
 * @retval FG_BAD_PARAMETER on any other parameter error
 * @retval FG_INVALID_TIME if any segments have negative duration
 * @retval FG_OUT_OF_LIMITS if reference value exceeds limits
 * @retval FG_OUT_OF_RATE_LIMITS if rate of change of reference exceeds limits
 * @retval FG_OUT_OF_ACCELERATION_LIMITS if acceleration exceeds limits
 */
enum FG_errno fgPpplInit(struct FG_limits *limits,
                         bool              pol_switch_auto,
                         bool              pol_switch_neg,
                         FG_float          initial_ref,
                         FG_float          acceleration1[FG_MAX_PPPLS],
                         uint32_t          acceleration1_num_els,
                         FG_float          acceleration2[FG_MAX_PPPLS],
                         uint32_t          acceleration2_num_els,
                         FG_float          acceleration3[FG_MAX_PPPLS],
                         uint32_t          acceleration3_num_els,
                         FG_float          rate2        [FG_MAX_PPPLS],
                         uint32_t          rate2_num_els,
                         FG_float          rate4        [FG_MAX_PPPLS],
                         uint32_t          rate4_num_els,
                         FG_float          ref4         [FG_MAX_PPPLS],
                         uint32_t          ref4_num_els,
                         FG_float          duration4    [FG_MAX_PPPLS],
                         uint32_t          duration4_num_els,
                         union  FG_pars   *pars,
                         struct FG_error  *error);



/*!
 * Real-time function to generate a PPPL reference.
 *
 * @param[in]  pars             Pointer to fg_pars union containing pppl parameter struct.
 * @param[in]  func_time        Time within the function.
 * @param[out] ref              Pointer to reference value.
 *
 * @retval FG_GEN_PRE_FUNC      if time is before the start of the function.
 * @retval FG_GEN_DURING_FUNC   if time is during the function.
 * @retval FG_GEN_POST_FUNC     if time is after the end of the function.
 */
enum FG_func_status fgPpplRT(union FG_pars *pars, FG_float func_time, float *ref);

#ifdef __cplusplus
}
#endif

#endif

// EOF
