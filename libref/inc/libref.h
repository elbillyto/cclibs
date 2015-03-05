/*!
 * @file      libref.h
 * @brief     Reference management library top-level header file
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
 * This file is part of libref.
 *
 * libref is free software: you can redistribute it and/or modify it under the
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

#ifndef LIBREF_H
#define LIBREF_H

// Libref enum constants

#define REF_PULSE_LEN       5
#define MAX_MAX_CYC_SELS    33

/*!
 * Reference state
 */
enum ref_state
{
    REF_OFF,
    REF_POL_SWITCHING,
    REF_TO_OFF,
    REF_TO_STANDBY,
    REF_TO_CYCLING,
    REF_TO_IDLE,
    REF_DIRECT,
    REF_ON_STANDBY,
    REF_CYCLING,
    REF_ECONOMY,
    REF_IDLE,
    REF_ARMED,
    REF_RUNNING,
    REF_PAUSED,
};

enum ref_fg_types
{
    REF_FG_NONE,
    REF_FG_PLEP,
    REF_FG_RAMP,
    REF_FG_PPPL,
    REF_FG_TABLE,
    REF_FG_STEPS,
    REF_FG_SQUARE,
    REF_FG_SINE,
    REF_FG_COSINE,
    REF_FG_LTRIM,
    REF_FG_CTRIM,
    REF_FG_PULSE,
};

/*!
 * Include header files needed for struct ref_mgr
 */
#include <stdint.h>
#include <stdbool.h>
#include <libref_vars.h>
#include <libref_pars.h>
#include <libfg.h>
#include <libreg.h>

/*!
 * Declare libref structures
 */
 union ref_fg_union
 {
    struct fg_plep          plep;
    struct fg_pppl          pppl;
    struct fg_pulse         pulse[REF_PULSE_LEN];
    struct fg_ramp          ramp;
    struct fg_table         table;
    struct fg_test          test;
    struct fg_trim          trim;
 };

 struct ref_fg_armed
 {
     enum reg_mode           reg_mode;                                          //!< Regulation mode
     enum ref_fg_types       fg_type;                                           //!< Function type
     struct fg_meta          fg_meta;                                           //!< Fg meta data for all possible cyc_sels and ref_pulse indexes
 };

struct ref_mgr
{
    enum ref_state              ref_state;                                      //!< Reference state

    uint32_t                    max_cyc_sel;                                    //!< Maximum cycle selector
    uint32_t                    max_table_points;                               //!< Maximum number of points in a table


    struct ref_mgr_ref_fg
    {
        struct ref_fg_armed     armed[MAX_MAX_CYC_SELS][REF_PULSE_LEN];         //!< Armed function meta data
        union ref_fg_union      fg_union[MAX_MAX_CYC_SELS];                     //!< Array of pointers to fg unions
        struct ref_fg_armed     next_and_active[2];                              //!< Array of active and next functions
        struct ref_fg_armed    *next;                                            //!< Pointer to next ref_fg in ref_fg_next_and_active
        struct ref_fg_armed    *active;                                          //!< Pointer to next ref_fg in ref_fg_next_and_active
    } ref_fg;

    struct ref_pars         pars;                                           //!< Pointers to libref parameters at the application level
};

/*!
 * Include all libref header files
 */
#include <libref/state.h>

#endif // LIBREF_H

// EOF
