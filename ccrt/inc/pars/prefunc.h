/*!
 * @file  ccrt/inc/pars/prefunc.h
 *
 * @brief ccrt PREFUNC parameters
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

#ifndef CCPARS_PREFUNC_H
#define CCPARS_PREFUNC_H

#include "ccPars.h"

// GLOBALS should be defined in the source file where global variables should be defined

#ifdef GLOBALS
#define CCPARS_PREFUNC_EXT
#else
#define CCPARS_PREFUNC_EXT extern
#endif

// Prefunc policies enum

enum prefunc_policy
{
    PREFUNC_RAMP,
    PREFUNC_UPMINMAX,
    PREFUNC_DOWNMAXMIN,
};

CCPARS_PREFUNC_EXT struct ccpars_enum enum_prefunc_policy[]
#ifdef GLOBALS
= {
    { PREFUNC_RAMP,         "RAMP"          },
    { PREFUNC_UPMINMAX,     "UPMINMAX"      },
    { PREFUNC_DOWNMAXMIN,   "DOWNMAXMIN"    },
    { 0,                     NULL           },
}
#endif
;

// Prefunc parameters structure

struct ccpars_prefunc
{
    enum prefunc_policy     policy;               // Pre-function policy
    float                   max;                  // Maximum level for UPMINMAX and DOWNMAXMIN pre-functions
    float                   minrms;               // Waiting level for pre/post-functions to minimise RMS losses
    float                   min;                  // Minimum level for UPMINMAX and DOWNMAXMIN pre-functions
    float                   plateau_duration;     // Pre-function plateau durations
};

CCPARS_PREFUNC_EXT struct ccpars_prefunc ccpars_prefunc
#ifdef GLOBALS
= {// Prefunc value        Parameter
    PREFUNC_RAMP,       // PREFUNC POLICY
    1.0,                // PREFUNC PREFUNC_MAX
    0.0,                // PREFUNC PREFUNC_MINRMS
    0.0,                // PREFUNC PREFUNC_MIN
    0.1,                // PREFUNC PLATEAU_DURATION
}
#endif
;

// Prefunc parameters description structure

CCPARS_PREFUNC_EXT struct ccpars prefunc_pars[]
#ifdef GLOBALS
= {// "Signal name"       type,  max_n_els, *enum,                       *value,                 num_prefuncs,cyc_sel_step,flags
    { "POLICY",           PAR_ENUM,  1,      enum_prefunc_policy, { .u = &ccpars_prefunc.policy           }, 1,     0,     PARS_RW|PARS_CFG },
    { "MAX",              PAR_FLOAT, 1,      NULL,                { .f = &ccpars_prefunc.max              }, 1,     0,     PARS_RW|PARS_CFG },
    { "MINRMS",           PAR_FLOAT, 1,      NULL,                { .f = &ccpars_prefunc.minrms           }, 1,     0,     PARS_RW|PARS_CFG },
    { "MIN",              PAR_FLOAT, 1,      NULL,                { .f = &ccpars_prefunc.min              }, 1,     0,     PARS_RW|PARS_CFG },
    { "PLATEAU_DURATION", PAR_FLOAT, 1,      NULL,                { .f = &ccpars_prefunc.plateau_duration }, 1,     0,     PARS_RW|PARS_CFG },
    { NULL }
}
#endif
;

#endif
// EOF

