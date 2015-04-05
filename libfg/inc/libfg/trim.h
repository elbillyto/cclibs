/*!
 * @file    libfg/trim.h
 * @brief   Generate linear and cubic trim functions
 *
 * Two types of trim function are supported, linear and cubic. It is possible to 
 * define the duration of the trim, or to go as fast as possible while respecting
 * the limits. For more details, see <a href="../pdf/TRIMS.pdf">TRIMS.pdf</a>.
 *
 * <h2>CTRIM or Cubic Trim (fg_trim_type::FG_TRIM_CUBIC)</h2>
 *
 * The Cubic Trim reference function is \f$r = at^{3} + ct\f$
 *
 * \image html  Cubic_Trim.png
 * \image latex Cubic_Trim.png "Cubic Trim" width=0.5\textwidth
 *
 * <h2>LTRIM or Linear Trim (fg_trim_type::FG_TRIM_LINEAR)</h2>
 *
 * The Linear Trim reference function is \f$r = ct\f$
 *
 * \image html  Linear_Trim.png
 * \image latex Linear_Trim.png "Linear Trim" width=0.5\textwidth
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

#ifndef LIBFG_TRIM_H
#define LIBFG_TRIM_H

#include "libfg.h"

// Types

/*!
 * Types of trim function
 */
enum FG_trim_type
{
    FG_TRIM_UNDEFINED,
    FG_TRIM_CUBIC,
    FG_TRIM_LINEAR
};

/*!
 * Trim function parameters
 */
struct FG_trim
{
    struct FG_meta      meta;           //!< Meta data for the armed function - this must be the first in the struct
    float               time_offset;    //!< Timebase offset for cubic.
    float               ref_offset;     //!< Reference offset.
    float               a;              //!< Coefficient for cubic term. \f$r = a \cdot t^3 + c \cdot t\f$
    float               c;              //!< Coefficient for constant term. \f$r = a \cdot t^3 + c \cdot t\f$
};

#ifdef __cplusplus
extern "C" {
#endif

// External functions

/*!
 * Initialise TRIM function.
 *
 * @param[in]  limits              Pointer to fgc_limits structure (or NULL if no limits checking required).
 * @param[in]  pol_switch_auto     Set true if polarity switch can be changed automatically.
 * @param[in]  pol_switch_neg      Set true if polarity switch is currently in the negative position.
 * @param[in]  initial_ref         Initial reference value.
 * @param[in]  final_ref           Final reference value.
 * @param[in]  type                Type of trim function (LINEAR or CUBIC).
 * @param[in]  duration            Function duration (Zero to go as fast as limits allow).
 * @param[out] pars                Pointer to fg_pars union containing trim parameter struct.
 * @param[out] error               Pointer to error information. Set to NULL if not required.
 *
 * @retval FG_OK on success
 * @retval FG_BAD_PARAMETER on parameter errors
 * @retval FG_OUT_OF_LIMITS if reference value exceeds limits
 * @retval FG_OUT_OF_RATE_LIMITS if rate of change of reference exceeds limits
 * @retval FG_OUT_OF_ACCELERATION_LIMITS if acceleration exceeds limits
 */
enum FG_errno fgTrimInit(struct FG_limits *limits, 
                         bool              pol_switch_auto,
                         bool              pol_switch_neg,
                         enum FG_trim_type type,
                         FG_float          initial_ref,
                         FG_float          final_ref,
                         FG_float          duration,
                         union  FG_pars   *pars,
                         struct FG_error  *error);



/*!
 * Real-time function to generate a CUBIC or LINEAR trim reference.
 *
 * @param[in]  pars             Pointer to fg_pars union containing trim parameter struct.
 * @param[in]  func_time        Time within the function.
 * @param[out] ref              Pointer to reference value.
 *
 * @retval FG_GEN_PRE_FUNC      if time is before the start of the function.
 * @retval FG_GEN_DURING_FUNC   if time is during the function.
 * @retval FG_GEN_POST_FUNC     if time is after the end of the function.
 */
enum FG_func_status fgTrimRT(union FG_pars *pars, FG_float func_time, FG_float *ref);

#ifdef __cplusplus
}
#endif

#endif

// EOF
