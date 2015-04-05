/*!
 * @file      libfg.h
 * @brief     Function Generation library top-level header file
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

#ifndef LIBFG_H
#define LIBFG_H

#include <stdlib.h>
#include <math.h>
#include <stdint.h>
#include <stdbool.h>

// libfg float typedef - Change to double if extra precision is needed

typedef float   FG_float;

// Constants

#define FG_CLIP_LIMIT_FACTOR    0.001           //!< Scale factor for user limits
#define FG_ERR_DATA_LEN         4               //!< error::data array length

/*!
 * Libfg Gen function return status
 */
enum FG_func_status
{
    FG_GEN_POST_FUNC,
    FG_GEN_DURING_FUNC,
    FG_GEN_PRE_FUNC
};

/*!
 * Libfg error numbers
 */
enum FG_errno
{
    FG_OK,
    FG_BAD_ARRAY_LEN,
    FG_BAD_PARAMETER,
    FG_INVALID_TIME,
    FG_OUT_OF_LIMITS,
    FG_OUT_OF_RATE_LIMITS,
    FG_OUT_OF_ACCELERATION_LIMITS
};

/*!
 * Libfg function polarity
 */
enum FG_func_pol                                //!< These can be used as a bit mask:
{                                               //!<   BOTH = POSITIVE | NEGATIVE
    FG_FUNC_POL_ZERO     = 0x0,                 //!< Function is entirely zero
    FG_FUNC_POL_POSITIVE = 0x1,                 //!< Function is entirely positive
    FG_FUNC_POL_NEGATIVE = 0x2,                 //!< Function is entirely negative
    FG_FUNC_POL_BOTH     = 0x3,                 //!< Function is both positive and negative
};

/*!
 * Libfg reference limits structure
 */
struct FG_limits
{
    FG_float            pos;                    //!< Positive reference limit
    FG_float            min;                    //!< Minimum absolute reference limit
    FG_float            neg;                    //!< Negative reference limit
    FG_float            rate;                   //!< Rate of change limit
    FG_float            acceleration;           //!< Acceleration limit
};

/*!
 * Libfg function error structure
 */
struct FG_error
{
    enum FG_errno       fg_errno;               //!< Function error number
    uint32_t            index;                  //!< Error index (e.g. table point or PPPL segment)
    float               data[FG_ERR_DATA_LEN];  //!< Error debug data - helps to diagnose error
};

/*!
 * Libfg function meta data structure
 *
 * Libfg supports 11 different types of function.
 *
 * Most start at time zero, but there are three exceptions, as indicated in the table.
 * Most end with a zero rate of change, but there are three exceptions, as indicated
 * in the table.
 *
 * +---------+------+-------------+----------------------+
 * |Source   |Func  |meta.time.   |meta.range.           |
 * |File     |Type  |start        |final_rate            |
 * +---------+------+-------------+----------------------+
 * |fgPlep.c |PLEP  |0.0          |final_rate            |
 * |fgPppl.c |PPPL  |0.0          |rate4[rate4_num_els-1]|
 * |fgPulse.c|PULSE |-lin_rate/acc|linear_rate           |
 * |fgRamp.c |RAMP  |start_time   |0.0                   |
 * |fgTablec |TABLE |time[0]      |0.0                   |
 * |fgTest.c |COSINE|0.0          |0.0                   |
 * |fgTest.c |SINE  |0.0          |0.0                   |
 * |fgTest.c |SQUARE|0.0          |0.0                   |
 * |fgTest.c |STEPS |0.0          |0.0                   |
 * |fgTrim.c |CTRIM |0.0          |0.0                   |
 * |fgTrim.c |LTRIM |0.0          |0.0                   |
 * +---------+------+-------------+----------------------+

 * Function time is divided into three phases:
 *
 *  1. PRE_FUNC    :                    func_time <  meta.time.start
 *  2. DURING_FUNC : meta.time.start <= func_time <  meta.time.end
 *  3. POST_FUNC   :                    func_time >= meta.time.end
 */
struct FG_meta
{
    enum FG_func_pol    polarity;               //!< Function polarity
    bool                limits_inverted;        //!< Function was checked against inverted limits

    struct FG_meta_time
    {
        FG_float        start;                  //!< Time of start of function
        FG_float        end;                    //!< Time of end of function
        FG_float        duration;               //!< Function duration (end - start)
    } time;

    struct FG_meta_range
    {
        FG_float        initial_ref;            //!< Reference value at start of function
        FG_float        final_ref;              //!< Reference value at the end of the function
        FG_float        min_ref;                //!< Minimum value of the function
        FG_float        max_ref;                //!< Maximum value of the function
        FG_float        final_rate;             //!< Rate of change of reference value at the end of the function
    } range;

    struct FG_meta_limits
    {
        FG_float        min;                    //!< Minimum function limit
        FG_float        max;                    //!< Maximum function limit
        FG_float        rate;                   //!< Maximum absolute rate of change limit
        FG_float        acceleration;           //!< Maximum absolute acceleration limit
    } limits;
};

/*!
 * Declare typedef for the union FG_pars so the reference function header files (libfg/plep.h etc...)
 * can make forward references to this union before it is declared below in this header file.
 */
typedef union FG_pars FG_pars;

/*!
 * Include all libfg header files
 */
#include <libfg/plep.h>
#include <libfg/pppl.h>
#include <libfg/pulse.h>
#include <libfg/ramp.h>
#include <libfg/table.h>
#include <libfg/test.h>
#include <libfg/trim.h>

/*!
 * Libfg union of meta and all function parameter structs.
 *
 * This is needed in all the reference function header files (libfg/plep.h etc...)
 * so a typedef must be declared above to allow forward references to this union.
 */
 union FG_pars
 {
    struct FG_meta      meta;
    struct FG_plep      plep;
    struct FG_pppl      pppl;
    struct FG_pulse     pulse;
    struct FG_ramp      ramp;
    struct FG_table     table;
    struct FG_test      test;
    struct FG_trim      trim;
 };


 /*
  * Declare typedefs for pointers to real-time Libfg reference generation functions
  */
 typedef enum FG_func_status (*FG_FuncRT) (union FG_pars *, FG_float, FG_float *);

// External functions

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * Reset all the meta data fields.
 *
 * This is a private function, used by the libfg initialisation functions.
 *
 * When a function is initialised, the meta data structure is filled with a summary of the function's
 * characteristics, including the delay, duration, min/max and start and end values.
 * In addition, an error structure can be filled in to help understand why a function
 * failed to arm.
 *
 * @param[in]  initial_ref  Initial value for the start, end, min and max.
 * @param[out] meta         Pointer to fg_meta data structure to initialise.
 * @param[in]  local_error  Pointer to fg_error structure to use if error is NULL (can also be NULL).
 * @param[out] error        Pointer to fg_error structure to initialise (can be NULL).
 *
 * @retval     Pointer to the fg_error structure that was reset
 *             (<em>meta</em> or <em>local_errora</em> if error is NULL).
 */
struct FG_error *fgResetMeta(FG_float         initial_ref,
                             struct FG_meta  *meta,
                             struct FG_error *local_error,
                             struct FG_error *error);



/*!
 * Set the meta min and max fields
 *
 * This is a private function, used by the libfg initialisation functions.
 *
 * @param[in]     ref       Value for reference to check against meta min and max.
 * @param[in,out] meta      Pointer to fg_meta structure.
 */
void fgSetMinMax(FG_float ref, struct FG_meta *meta);



/*!
 * Set the function's meta polarity and limits (if provided)
 *
 * This is a private function, used by libfg initialisation functions.
 *
 * @param[in]     pol_switch_auto True if polarity switch can be changed automatically.
 * @param[in]     pol_switch_neg  True if polarity switch is currently in the negative position.
 * @param[in]     end_time        Time of end of function.
 * @param[in]     limits          Pointer to fg_limits structure (can be NULL).
 * @param[in,out] meta            Pointer to fg_meta structure.
 */
void fgSetMeta(bool              pol_switch_auto,
               bool              pol_switch_neg,
               FG_float          end_time,
               struct FG_limits *limits,
               struct FG_meta   *meta);



/*!
 * Check function reference value, rate and acceleration against the supplied limits.
 *
 * This is a private function, used by libfg initialisation functions.
 *
 * When a function is initialised, this is called to check the function value,
 * rate and acceleration (in that order) against the limits in meta (set by fgSetFuncPolarity).
 * The function returns the status of the first limit that is exceeded and the return value
 * indicates the type of error (REF, RATE, ACCELERATION). The error::data array
 * provides extra information about limit that was exceeded:
 * <PRE>
 * |RETURN VALUE                 |ERROR.DATA[0]|ERROR.DATA[1]|ERROR.DATA[2]|
 * |-----------------------------|-------------|-------------|-------------|
 * |FG_OUT_OF_LIMITS             |ref          |min          |max          |
 * |FG_OUT_OF_RATE_LIMITS        |rate         |rate_limit   |0.0          |
 * |FG_OUT_OF_ACCELERATION_LIMITS|acceleration |acc_limit    |0.0          |
 * |-----------------------------|-------------|-------------|-------------|
 * </PRE>
 *
 * @param[in]  ref           Reference value.
 * @param[in]  rate          Rate of change of reference.
 * @param[in]  acceleration  Acceleration of reference.
 * @param[in]  meta          Pointer to fg_meta structure containing limits.
 * @param[out] error         Pointer to fg_error structure used to return detailed error codes.
 *
 * @retval FG_OK on success.
 * @retval FG_OUT_OF_LIMITS if function value is out of limits.
 * @retval FG_OUT_OF_RATE_LIMITS if function rate of change is out of limits.
 * @retval FG_OUT_OF_ACCELERATION_LIMITS if function acceleration is out of limits.
 */
enum FG_errno fgCheckRef(FG_float          ref,
                         FG_float          rate,
                         FG_float          acceleration,
                         struct FG_meta   *meta,
                         struct FG_error  *error);

#ifdef __cplusplus
}
#endif

#endif

// EOF
