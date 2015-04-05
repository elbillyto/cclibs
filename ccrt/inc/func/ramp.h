/*!
 * @file  ccrt/inc/func/ramp.h
 *
 * @brief ccrt RAMP function parameters
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

#ifndef CCPARS_RAMP_H
#define CCPARS_RAMP_H

#include "ccRt.h"
#include "ccPars.h"

// GLOBALS is defined in source file where global variables should be defined

#ifdef GLOBALS
#define CCPARS_RAMP_EXT
#else
#define CCPARS_RAMP_EXT extern
#endif

// RAMP data structure

struct ccpars_ramp
{
    // ccrt RAMP parameters

    float                       initial_ref;                    // Initial reference
    float                       final_ref;                      // Final reference
    float                       acceleration;                   // Acceleration of the 1st parabolic segment. Absolute value is used.
    float                       linear_rate;                    // Maximum linear rate. Absolute value is used.
    float                       deceleration;                   // Deceleration of the 2nd parabolic segment. Absolute value is used.
};

CCPARS_RAMP_EXT struct ccpars_ramp ccpars_ramp[CC_NUM_CYC_SELS]
#ifdef GLOBALS
= {// Default value             Parameter
    {   0.0,                 // RAMP INITIAL_REF
        1.0,                 // RAMP FINAL_REF
        4.0,                 // RAMP ACCELERATION
        1.0,                 // RAMP LINEAR_RATE
        6.0   }              // RAMP DECELERTION
}
#endif
;

// RAMP data description structure

CCPARS_RAMP_EXT struct CCpars   ramp_pars[]
#ifdef GLOBALS
= {// "Signal name"   type,     max_n_els,*enum,        *value,                   num_defaults      cyc_sel_step     flags
    { "INITIAL_REF",  PAR_FLOAT,    1,     NULL, { .f = &ccpars_ramp[0].initial_ref  }, 1, sizeof(struct ccpars_ramp), PARS_RW|PARS_REF },
    { "FINAL_REF",    PAR_FLOAT,    1,     NULL, { .f = &ccpars_ramp[0].final_ref    }, 1, sizeof(struct ccpars_ramp), PARS_RW|PARS_REF },
    { "ACCELERATION", PAR_FLOAT,    1,     NULL, { .f = &ccpars_ramp[0].acceleration }, 1, sizeof(struct ccpars_ramp), PARS_RW|PARS_REF },
    { "LINEAR_RATE",  PAR_FLOAT,    1,     NULL, { .f = &ccpars_ramp[0].linear_rate  }, 1, sizeof(struct ccpars_ramp), PARS_RW|PARS_REF },
    { "DECELERATION", PAR_FLOAT,    1,     NULL, { .f = &ccpars_ramp[0].deceleration }, 1, sizeof(struct ccpars_ramp), PARS_RW|PARS_REF },
    { NULL }
}
#endif
;

#endif
// EOF

