/*---------------------------------------------------------------------------------------------------------*\
  File:     cctest/inc/func/test.h                                                      Copyright CERN 2015

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

  Purpose:  Structure for test function (SINE, COSINE, STEPS, SQUARE) parameters

  Authors:  Quentin.King@cern.ch
\*---------------------------------------------------------------------------------------------------------*/

#ifndef CCPARS_TEST_H
#define CCPARS_TEST_H

#include "ccTest.h"
#include "ccPars.h"

// GLOBALS is defined in source file where global variables should be defined

#ifdef GLOBALS
#define CCPARS_TEST_EXT
#else
#define CCPARS_TEST_EXT extern
#endif

// Test parameters structure

struct CCpars_test
{
    // cctest TEST parameters

    float                       initial_ref;                    // Initial reference
    enum FG_test_type           type;                           // Type of test function
    float                       amplitude_pp;                   // Ref peak-to-peak amplitude
    float                       num_cycles;                     // Number of cycles/steps. This is rounded to the nearest integer.
    float                       period;                         // Period
    enum REG_enabled_disabled   use_window;                     // Window control: true to use window for sine & cosine.
    enum REG_enabled_disabled   use_exp_decay;                  // Exp decay control: if window is active this can enable an exp decay
};

CCPARS_TEST_EXT struct CCpars_test ccpars_test[CC_NUM_CYC_SELS]
#ifdef GLOBALS
= {// Default value                Parameter
    {   0.0,                    // TEST INITIAL_REF
        FG_TEST_COSINE,         // Overwritten by init function (SINE, COSINE, STEPS or SQUARE)
        2.0,                    // TEST AMPLITUDE_PP
        3.0,                    // TEST NUM_CYCLES
        2.0,                    // TEST PERIOD
        REG_ENABLED,            // TEST WINDOW
        REG_DISABLED     },     // TEST EXP_DECAY
}
#endif
;

// Test data description structure

CCPARS_TEST_EXT struct CCpars test_pars[]
#ifdef GLOBALS
= {// "Signal name"   type,     max_n_els,*enum,                         *value,                   num_defaults      cyc_sel_step   flags
    { "INITIAL_REF",  PAR_FLOAT,    1,     NULL,                  { .f = &ccpars_test[0].initial_ref  }, 1, sizeof(struct CCpars_test), 0 },
    { "AMPLITUDE_PP", PAR_FLOAT,    1,     NULL,                  { .f = &ccpars_test[0].amplitude_pp }, 1, sizeof(struct CCpars_test), 0 },
    { "NUM_CYCLES",   PAR_FLOAT,    1,     NULL,                  { .f = &ccpars_test[0].num_cycles   }, 1, sizeof(struct CCpars_test), 0 },
    { "PERIOD",       PAR_FLOAT,    1,     NULL,                  { .f = &ccpars_test[0].period       }, 1, sizeof(struct CCpars_test), 0 },
    { "WINDOW",       PAR_ENUM,     1,     enum_enabled_disabled, { .u = &ccpars_test[0].use_window   }, 1, sizeof(struct CCpars_test), 0 },
    { "EXP_DECAY",    PAR_ENUM,     1,     enum_enabled_disabled, { .u = &ccpars_test[0].use_exp_decay}, 1, sizeof(struct CCpars_test), 0 },
    { NULL }
}
#endif
;

#endif
// EOF
