/*---------------------------------------------------------------------------------------------------------*\
  File:     cctest/inc/func/pulse.h                                                     Copyright CERN 2015

  License:  This file is part of cctest.

            cctest is free software: you can redistribute it and/or modify
            it under the terms of the GNU Lesser General Public License as published by
            the Free Software Foundation, either version 3 of the License, or
            (at your option) any later version.

            This program is distributed in the hope that it will be useful,
            but WITHOUT ANY WARRANTY; without even the implied warranty of
            MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
            GNU Lesser General Public License for more details.

            You should have received a copy of the GNU Lesser General Public License
            along with this program.  If not, see <http://www.gnu.org/licenses/>.

  Purpose:  Structure for PULSE function parameters

  Authors:  Quentin.King@cern.ch
\*---------------------------------------------------------------------------------------------------------*/

#ifndef CCPARS_PULSE_H
#define CCPARS_PULSE_H

#include "ccTest.h"
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
    // cctest PULSE parameters

    float                       ref;                            // Pulse reference
    float                       acceleration;                   // Pulse acceleration to reach linear rate
    float                       linear_rate;                    // Pulse linear rate
    float                       duration;                       // Pulse duration
};

CCPARS_PULSE_EXT struct CCpars_pulse ccpars_pulse[CC_NUM_CYC_SELS]
#ifdef GLOBALS
= {// Default value                 Parameter
    {   1.0,                     // PULSE REF
        1.0,                     // PULSE ACCELERATION
        0.1,                     // PULSE LINEAR_RATE
        1.0   },                 // PULSE DURATION
}
#endif
;
// Pulse data description structure

CCPARS_PULSE_EXT struct CCpars   pulse_pars[]
#ifdef GLOBALS
= {// "Signal name", type,  max_n_els, *enum,        *value,                   num_defaults      cyc_sel_step      flags
    { "REF",         PAR_FLOAT, 1,      NULL, { .f = &ccpars_pulse[0].ref         }, 1, sizeof(struct CCpars_pulse), 0 },
    { "ACCELERATION",PAR_FLOAT, 1,      NULL, { .f = &ccpars_pulse[0].acceleration}, 1, sizeof(struct CCpars_pulse), 0 },
    { "LINEAR_RATE", PAR_FLOAT, 1,      NULL, { .f = &ccpars_pulse[0].linear_rate }, 1, sizeof(struct CCpars_pulse), 0 },
    { "DURATION",    PAR_FLOAT, 1,      NULL, { .f = &ccpars_pulse[0].duration    }, 1, sizeof(struct CCpars_pulse), 0 },
    { NULL }
}
#endif
;

#endif
// EOF
