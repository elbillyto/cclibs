/*!
 * @file  ccrt/inc/pars/warnings.h
 *
 * @brief ccrt WARNINGS parameters
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

#ifndef CCPARS_WARNINGS_H
#define CCPARS_WARNINGS_H

#include "ccPars.h"

// GLOBALS should be defined in the source file where global variables should be defined

#ifdef GLOBALS
#define CCPARS_WARNINGS_EXT
#else
#define CCPARS_WARNINGS_EXT extern
#endif

// Warnings parameters structure

struct ccpars_warnings
{
    enum REG_enabled_disabled   b_reg_err;                      // Field regulation error warning
    enum REG_enabled_disabled   i_reg_err;                      // Current regulation error warning
    enum REG_enabled_disabled   v_reg_err;                      // Voltage regulation error warning

    enum REG_enabled_disabled   i_rms;                          // RMS Current exceeds converter warning limit
    enum REG_enabled_disabled   i_rms_load;                     // RMS Current exceeds load warning limit
};

CCPARS_WARNINGS_EXT struct ccpars_warnings ccpars_warnings;

// Warnings parameters description structure

CCPARS_WARNINGS_EXT struct CCpars warnings_pars[]
#ifdef GLOBALS
= {// "Signal name"     type,  max_n_els, *enum,                       *value,                  num_warnings,cyc_sel_step,flags
    { "B_REG_ERR",      PAR_ENUM,  1,      enum_enabled_disabled, { .u = &ccpars_warnings.b_reg_err      }, 1,     0,     PARS_RO },
    { "I_REG_ERR",      PAR_ENUM,  1,      enum_enabled_disabled, { .u = &ccpars_warnings.i_reg_err      }, 1,     0,     PARS_RO },
    { "V_REG_ERR",      PAR_ENUM,  1,      enum_enabled_disabled, { .u = &ccpars_warnings.v_reg_err      }, 1,     0,     PARS_RO },
    { "I_RMS",          PAR_ENUM,  1,      enum_enabled_disabled, { .u = &ccpars_warnings.i_rms          }, 1,     0,     PARS_RO },
    { "I_RMS_LOAD",     PAR_ENUM,  1,      enum_enabled_disabled, { .u = &ccpars_warnings.i_rms_load     }, 1,     0,     PARS_RO },
    { NULL }
}
#endif
;

#endif

// EOF
