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
#include "libfg/trim.h"

// GLOBALS is defined in source file where global variables should be defined

#ifdef GLOBALS
#define CCPARS_PULSE_EXT
#else
#define CCPARS_PULSE_EXT extern
#endif

// Libfg TRIM parameter structures for PULSE

CCPARS_PULSE_EXT struct fg_trim fg_pulse[CC_NUM_CYC_SELS];

// Pulse data structure

struct ccpars_pulse
{
    // cctest PULSE parameters

    float                       time;                           // Start of pulse time
    float                       duration;                       // Pulse duration
    float                       ref;                            // Pulse reference
    float                       linear_rate;                    // Pulse ramp rate
};

CCPARS_PULSE_EXT struct ccpars_pulse ccpars_pulse[CC_NUM_CYC_SELS]
#ifdef GLOBALS
= {// Default value                 Parameter
    {   1.0,                     // PULSE TIME
        1.0,                     // PULSE DURATION
        0.0,                     // PULSE REF
        0.0   },                 // PULSE LINEAR_RATE
}
#endif
;
// Pulse data description structure

CCPARS_PULSE_EXT struct ccpars   pulse_pars[]
#ifdef GLOBALS
= {// "Signal name", type,  max_n_els, *enum,        *value,                num_defaults      cyc_sel_step      flags
    { "TIME",        PAR_FLOAT, 1,      NULL, { .f = &ccpars_pulse[0].time     }, 1, sizeof(struct ccpars_pulse), PARS_RW|PARS_REF },
    { "DURATION",    PAR_FLOAT, 1,      NULL, { .f = &ccpars_pulse[0].duration }, 1, sizeof(struct ccpars_pulse), PARS_RW|PARS_REF },
    { "REF",         PAR_FLOAT, 1,      NULL, { .f = &ccpars_pulse[0].ref      }, 1, sizeof(struct ccpars_pulse), PARS_RW|PARS_REF },
    { "LINEAR_RATE", PAR_FLOAT, 1,      NULL, { .f = &ccpars_pulse[0].ref      }, 1, sizeof(struct ccpars_pulse), PARS_RW|PARS_REF },
    { NULL }
}
#endif
;

#endif
// EOF
