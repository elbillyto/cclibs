/*!
 * @file  ccrt/inc/pars/postfunc.h
 *
 * @brief ccrt POSTFUNC parameters
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

#ifndef CCPARS_POSTFUNC_H
#define CCPARS_POSTFUNC_H

#include "ccPars.h"

// GLOBALS should be defined in the source file where global variables should be defined

#ifdef GLOBALS
#define CCPARS_POSTFUNC_EXT
#else
#define CCPARS_POSTFUNC_EXT extern
#endif

// Pre-function policies enum

enum postfunc_policy
{
    POSTFUNC_HOLD,
    POSTFUNC_MINRMS,
};

CCPARS_POSTFUNC_EXT struct CCpars_enum enum_postfunc_policy[]
#ifdef GLOBALS
= {
    { POSTFUNC_MINRMS,       "MINRMS"       },
    { POSTFUNC_HOLD,         "HOLD"         },
    { 0,                      NULL          },
}
#endif
;

// Postfunc parameters structure

struct ccpars_postfunc
{
    enum postfunc_policy    policy;               // Postfunc policy
};

CCPARS_POSTFUNC_EXT struct ccpars_postfunc ccpars_postfunc
#ifdef GLOBALS
= {// Postfunc value        Parameter
    POSTFUNC_MINRMS,     // POSTFUNC POLICY
}
#endif
;

// Postfunc parameters description structure

CCPARS_POSTFUNC_EXT struct CCpars postfunc_pars[]
#ifdef GLOBALS
= {// "Signal name"       type,  max_n_els, *enum,                        *value,        num_postfuncs,cyc_sel_step,flags
    { "POLICY",           PAR_ENUM,  1,      enum_postfunc_policy, { .u = &ccpars_postfunc.policy }, 1,     0,      PARS_RW|PARS_CFG },
    { NULL }
}
#endif
;

#endif
// EOF

