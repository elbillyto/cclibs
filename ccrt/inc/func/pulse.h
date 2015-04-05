/*!
 * @file  ccrt/inc/func/pulse.h
 *
 * @brief ccrt PULSE function parameters
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

#ifndef CCPARS_PULSE_H
#define CCPARS_PULSE_H

#include "ccRt.h"
#include "ccPars.h"

// GLOBALS is defined in source file where global variables should be defined

#ifdef GLOBALS
#define CCPARS_PULSE_EXT
#else
#define CCPARS_PULSE_EXT extern
#endif

// Pulse data structure

struct CCpars_pulse
{
    // ccrt PULSE parameters

    uint32_t                    index;                                // Index in time/duration/ref/linear_rate arrays
    float                       ref         [REF_PULSE_LEN];          // Pulse reference
    float                       acceleration[REF_PULSE_LEN];          // Pulse acceleration to reach linear rate
    float                       linear_rate [REF_PULSE_LEN];          // Pulse linear rate
    float                       duration    [REF_PULSE_LEN];          // Pulse duration
};

CCPARS_PULSE_EXT struct CCpars_pulse ccpars_pulse[CC_NUM_CYC_SELS]
#ifdef GLOBALS
= {// Default value                 Parameter
    {         0,                 // PULSE INDEX
        {   0.0   },             // PULSE REF
        {   1.0   },             // PULSE ACCELERATION
        {   0.0   }              // PULSE LINEAR_RATE
        {   1.0   },             // PULSE DURATION
    }
}
#endif
;

// Pulse parameters enum to allow access to named fields

enum pulse_pars_index_enum
{
    PULSE_INDEX,
    PULSE_REF,
    PULSE_ACCELERATION,
    PULSE_LINEAR_RATE,
    PULSE_DURATION,
};

// Pulse data description structure

CCPARS_PULSE_EXT struct CCpars   pulse_pars[]
#ifdef GLOBALS
= {// "Signal name", type,         max_n_els,    *enum,        *value,                    num_defaults      cyc_sel_step      flags
    { "INDEX",       PAR_UNSIGNED, 1,             NULL, { .u = &ccpars_pulse[0].index        }, 1, sizeof(struct CCpars_pulse), PARS_RW|PARS_REF },
    { "REF",         PAR_FLOAT,    REF_PULSE_LEN, NULL, { .f =  ccpars_pulse[0].ref          }, 1, sizeof(struct CCpars_pulse), PARS_RW|PARS_REF },
    { "ACCELERATION",PAR_FLOAT,    REF_PULSE_LEN, NULL, { .f =  ccpars_pulse[0].acceleration }, 1, sizeof(struct CCpars_pulse), PARS_RW|PARS_REF },
    { "LINEAR_RATE", PAR_FLOAT,    REF_PULSE_LEN, NULL, { .f =  ccpars_pulse[0].linear_rate  }, 1, sizeof(struct CCpars_pulse), PARS_RW|PARS_REF },
    { "DURATION",    PAR_FLOAT,    REF_PULSE_LEN, NULL, { .f =  ccpars_pulse[0].duration     }, 1, sizeof(struct CCpars_pulse), PARS_RW|PARS_REF },
    { NULL }
}
#endif
;

#endif
// EOF
