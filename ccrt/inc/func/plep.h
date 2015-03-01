/*!
 * @file  ccrt/inc/func/plep.h
 *
 * @brief ccrt PLEP function parameters
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

#ifndef CCPARS_PLEP_H
#define CCPARS_PLEP_H

#include "ccRt.h"
#include "ccPars.h"
#include "libfg/plep.h"

// GLOBALS is defined in source file where global variables should be defined

#ifdef GLOBALS
#define CCPARS_PLEP_EXT
#else
#define CCPARS_PLEP_EXT extern
#endif

// Libfg PLEP parameter structures

CCPARS_PLEP_EXT struct fg_plep fg_plep[CC_NUM_CYC_SELS];

// PLEP data structure

struct ccpars_plep
{
    // cctest PLEP parameters

    float                       initial_ref;                    // Initial reference
    float                       final_ref;                      // Final reference
    float                       acceleration;                   // Acceleration of the parabolic segments (absolute value is used)
    float                       linear_rate;                    // Maximum linear rate (absolute value is used)
    float                       exp_tc;                         // Exponential time constant
    float                       exp_final;                      // End reference of exponential segment (can be zero)
};

CCPARS_PLEP_EXT struct ccpars_plep ccpars_plep[CC_NUM_CYC_SELS]
#ifdef GLOBALS
= {// Default value           Parameter
    { 0.0,                 // PLEP INITIAL_REF
      1.0,                 // PLEP FINAL_REF
      1.0,                 // PLEP ACCELERATION
      1.0,                 // PLEP LINEAR_RATE
      0.0,                 // PLEP EXP_TC
      0.0 }                // PLEP EXP_FINAL
}
#endif
;

// PLEP data description structure

CCPARS_PLEP_EXT struct ccpars   plep_pars[]
#ifdef GLOBALS
= {// "Signal name"   type,     max_n_els,*enum,        *value,                   num_defaults      cyc_sel_step     flags
    { "INITIAL_REF",  PAR_FLOAT,    1,     NULL, { .f = &ccpars_plep[0].initial_ref  }, 1, sizeof(struct ccpars_plep), PARS_RW|PARS_REF },
    { "FINAL_REF",    PAR_FLOAT,    1,     NULL, { .f = &ccpars_plep[0].final_ref    }, 1, sizeof(struct ccpars_plep), PARS_RW|PARS_REF },
    { "ACCELERATION", PAR_FLOAT,    1,     NULL, { .f = &ccpars_plep[0].acceleration }, 1, sizeof(struct ccpars_plep), PARS_RW|PARS_REF },
    { "LINEAR_RATE",  PAR_FLOAT,    1,     NULL, { .f = &ccpars_plep[0].linear_rate  }, 1, sizeof(struct ccpars_plep), PARS_RW|PARS_REF },
    { "EXP_TC",       PAR_FLOAT,    1,     NULL, { .f = &ccpars_plep[0].exp_tc       }, 1, sizeof(struct ccpars_plep), PARS_RW|PARS_REF },
    { "EXP_FINAL",    PAR_FLOAT,    1,     NULL, { .f = &ccpars_plep[0].exp_final    }, 1, sizeof(struct ccpars_plep), PARS_RW|PARS_REF },
    { NULL }
}
#endif
;

#endif
// EOF

