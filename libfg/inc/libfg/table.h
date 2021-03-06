/*!
 * @file    libfg/table.h
 * @brief   Generate linearly interpolated table functions.
 *
 * This function is provided with a list of reference values and times that the
 * reference values take effect, i.e. it calculates the reference by table lookup.
 *
 * Note that if the calling application Using linear interpolation results in an error between the actual linear
 * reference and the ideal parabolic reference. It can be shown that for parabola
 * \f$y = \frac{at^{2}}{2}\f$, the maximum interpolation error for segment
 * \f$0 \leq t \leq T\f$ is \f$\epsilon_{max} = \frac{aT^{2}}{8}\f$.
 *
 * \image html  Interpolation_error.png
 * \image latex Interpolation_error.png "Linear interpolation error" width=0.5\textwidth
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

#ifndef LIBFG_TABLE_H
#define LIBFG_TABLE_H

#include "libfg.h"

/*!
 * Table function parameters
 */
struct FG_table
{
    struct FG_meta meta;                //!< Meta data for the armed function - this must be the first in the struct
    uint32_t       seg_idx;             //!< Segment index.
    uint32_t       prev_seg_idx;        //!< Previous segment index for which gradient was calculated.
    uint32_t       num_points;          //!< Number of points in table.
    FG_float      *ref;                 //!< Table reference values.
    FG_float      *time;                //!< Table time values.
    FG_float       seg_grad;            //!< Gradient of reference for segment fg_table::prev_seg_idx.
};

#ifdef __cplusplus
extern "C" {
#endif

// External functions

/*!
 * Initialise TABLE function.
 *
 * @param[in]  limits             Pointer to fgc_limits structure (or NULL if no limits checking required).
 * @param[in]  pol_switch_auto    True if polarity switch can be changed automatically.
 * @param[in]  pol_switch_neg     True if polarity switch is currently in the negative position.
 * @param[in]  min_time_step      Minimum time between points in the table.
 * @param[in] *ref                Array of reference values.
 * @param[in]  ref_num_els        Number of elements in reference array.
 * @param[in] *time               Array of time values.
 * @param[in]  time_num_els       Number of elements in time array.
 * @param[in] *armed_ref          Array of armed reference values (set to NULL if ref and time to be used).
 * @param[in] *armed_time         Array of armed time values (set to NULL if ref and time to be used).
 * @param[out] pars               Pointer to fg_pars union containing table parameter struct.
 * @param[out] error              Pointer to error information. Set to NULL if not required.
 *
 * @retval FG_OK on success
 * @retval FG_BAD_ARRAY_LEN if there are less than two points, or ref and time arrays are different lengths
 * @retval FG_INVALID_TIME if time values are not at least min_time_step seconds apart
 * @retval FG_OUT_OF_LIMITS if reference value exceeds limits
 * @retval FG_OUT_OF_RATE_LIMITS if rate of change of reference exceeds limits
 * @retval FG_OUT_OF_ACCELERATION_LIMITS if acceleration exceeds limits
 */
enum FG_errno fgTableInit(struct FG_limits *limits,
                          bool              pol_switch_auto,
                          bool              pol_switch_neg,
                          FG_float          min_time_step,
                          FG_float         *ref,
                          uint32_t          ref_num_els,
                          FG_float         *time,
                          uint32_t          time_num_els,
                          FG_float         *armed_ref,
                          FG_float         *armed_time,
                          union  FG_pars *pars,
                          struct FG_error  *error);



/*!
 * Real-time function to generate a TABLE reference.
 *
 * @param[in]  pars             Pointer to fg_pars union containing pulse parameter struct.
 * @param[in]  func_time        Time within the function.
 * @param[out] ref              Pointer to reference value.
 *
 * @retval FG_GEN_PRE_FUNC      if time is before the start of the function.
 * @retval FG_GEN_DURING_FUNC   if time is during the function.
 * @retval FG_GEN_POST_FUNC     if time is after the end of the function.
 */
enum FG_func_status fgTableRT(union FG_pars *pars, FG_float func_time, FG_float *ref);

#ifdef __cplusplus
}
#endif

#endif

// EOF
