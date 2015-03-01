/*!
 * @file  ccrt/inc/pars/direct.h
 *
 * @brief ccrt DIRECT parameters
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

#ifndef CCPARS_DIRECT_H
#define CCPARS_DIRECT_H

#include "ccPars.h"

// GLOBALS should be defined in the source file where global variables should be defined

#ifdef GLOBALS
#define CCPARS_DIRECT_EXT
#else
#define CCPARS_DIRECT_EXT extern
#endif

// Direct parameters structure

struct ccpars_direct
{
    float                   b_ref;                // Direct field reference
    float                   i_ref;                // Direct current reference
    float                   v_ref;                // Direct voltage reference
};

CCPARS_DIRECT_EXT struct ccpars_direct ccpars_direct
#ifdef GLOBALS
= {// Direct value        Parameter
        0.0,                // DIRECT B_REF
        0.0,                // DIRECT I_REF
        0.0,                // DIRECT V_REF
}
#endif
;

// Direct parameters description structure

CCPARS_DIRECT_EXT struct ccpars direct_pars[]
#ifdef GLOBALS
= {// "Signal name" type,  max_n_els, *enum,        *value,       num_directs,cyc_sel_step,flags
    { "B_REF",      PAR_FLOAT, 1,      NULL, { .f = &ccpars_direct.b_ref }, 1,      0,     PARS_RW|PARS_CFG },
    { "I_REF",      PAR_FLOAT, 1,      NULL, { .f = &ccpars_direct.i_ref }, 1,      0,     PARS_RW|PARS_CFG },
    { "V_REF",      PAR_FLOAT, 1,      NULL, { .f = &ccpars_direct.v_ref }, 1,      0,     PARS_RW|PARS_CFG },
    { NULL }
}
#endif
;

#endif
// EOF

