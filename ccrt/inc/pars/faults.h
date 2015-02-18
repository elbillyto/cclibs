/*!
 * @file  ccrt/inc/pars/faults.h
 *
 * @brief ccrt FAULTS parameters
 *
 * <h2>Copyright</h2>
 *
 * Copyright CERN 2015. This project is released under the GNU Lesser General
 * Public License version 3.
 *
 * <h2>License</h2>
 *
 * This file is part of ccrt.
 *
 * ccrt is free software: you can redistribute it and/or modify it under the
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

#ifndef CCPARS_FAULTS_H
#define CCPARS_FAULTS_H

#include "ccPars.h"

// GLOBALS should be defined in the source file where global variables should be defined

#ifdef GLOBALS
#define CCPARS_FAULTS_EXT
#else
#define CCPARS_FAULTS_EXT extern
#endif

// Faults parameters structure

struct ccpars_faults
{
    enum reg_enabled_disabled   b_meas_invalid;                 // Field measurement invalid
    enum reg_enabled_disabled   b_meas_limit;                   // Field measurement limit exceeded
    enum reg_enabled_disabled   b_reg_err;                      // Field regulation error fault

    enum reg_enabled_disabled   i_meas_invalid;                 // Current measurement invalid
    enum reg_enabled_disabled   i_meas_limit;                   // Current measurement limit exceeded
    enum reg_enabled_disabled   i_reg_err;                      // Current regulation error fault

    enum reg_enabled_disabled   v_reg_err;                      // Voltage regulation error fault

    enum reg_enabled_disabled   i_rms;                          // RMS Current exceeds converter fault limit
    enum reg_enabled_disabled   i_rms_load;                     // RMS Current exceeds load fault limit

    enum reg_enabled_disabled   polswitch;                      // Polarity switch fault
};

CCPARS_FAULTS_EXT struct ccpars_faults ccpars_faults;

// Faults parameters description structure

CCPARS_FAULTS_EXT struct ccpars faults_pars[]
#ifdef GLOBALS
= {// "Signal name"     type,  max_n_els, *enum,                       *value,                  num_faults,cyc_sel_step,flags
    { "B_MEAS_INVALID", PAR_ENUM,  1,      enum_enabled_disabled, { .u = &ccpars_faults.b_meas_invalid }, 1,     0,     PARS_RO },
    { "B_MEAS_LIMIT",   PAR_ENUM,  1,      enum_enabled_disabled, { .u = &ccpars_faults.b_meas_limit   }, 1,     0,     PARS_RO },
    { "B_REG_ERR",      PAR_ENUM,  1,      enum_enabled_disabled, { .u = &ccpars_faults.b_reg_err      }, 1,     0,     PARS_RO },
    { "I_MEAS_INVALID", PAR_ENUM,  1,      enum_enabled_disabled, { .u = &ccpars_faults.i_meas_invalid }, 1,     0,     PARS_RO },
    { "I_MEAS_LIMIT",   PAR_ENUM,  1,      enum_enabled_disabled, { .u = &ccpars_faults.i_meas_limit   }, 1,     0,     PARS_RO },
    { "I_REG_ERR",      PAR_ENUM,  1,      enum_enabled_disabled, { .u = &ccpars_faults.i_reg_err      }, 1,     0,     PARS_RO },
    { "V_REG_ERR",      PAR_ENUM,  1,      enum_enabled_disabled, { .u = &ccpars_faults.v_reg_err      }, 1,     0,     PARS_RO },
    { "I_RMS",          PAR_ENUM,  1,      enum_enabled_disabled, { .u = &ccpars_faults.i_rms          }, 1,     0,     PARS_RO },
    { "I_RMS_LOAD",     PAR_ENUM,  1,      enum_enabled_disabled, { .u = &ccpars_faults.i_rms_load     }, 1,     0,     PARS_RO },
    { "POLSWITCH",      PAR_ENUM,  1,      enum_enabled_disabled, { .u = &ccpars_faults.polswitch      }, 1,     0,     PARS_RO },
    { NULL }
}
#endif
;

#endif

// EOF
