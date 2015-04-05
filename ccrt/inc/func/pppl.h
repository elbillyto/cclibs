/*!
 * @file  ccrt/inc/func/pppl.h
 *
 * @brief ccrt PPPL function parameters
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

#ifndef CCPARS_PPPL_H
#define CCPARS_PPPL_H

#include "ccRt.h"
#include "ccPars.h"

// GLOBALS is defined in source file where global variables should be defined

#ifdef GLOBALS
#define CCPARS_PPPL_EXT
#else
#define CCPARS_PPPL_EXT extern
#endif

// PPPL parameters structure

struct CCpars_pppl
{
    // ccrt PPPL parameters

    float                       initial_ref;                    // Initial reference
    float                       acceleration1[FG_MAX_PPPLS];    // Acceleration of first  (parabolic) segment.
    float                       acceleration2[FG_MAX_PPPLS];    // Acceleration of second (parabolic) segment.
    float                       acceleration3[FG_MAX_PPPLS];    // Acceleration of third  (parabolic) segment.
    float                       rate2        [FG_MAX_PPPLS];    // Rate of change at start of second (parabolic) segment.
    float                       rate4        [FG_MAX_PPPLS];    // Rate of change of fourth (linear) segment.
    float                       ref4         [FG_MAX_PPPLS];    // Reference at start of fourth (linear) segment.
    float                       duration4    [FG_MAX_PPPLS];    // Duration of fourth (linear) segment.
};

CCPARS_PPPL_EXT struct CCpars_pppl ccpars_pppl[CC_NUM_CYC_SELS]
#ifdef GLOBALS
= {//   Default value           Parameter
    {    0.0,                // PPPL INITIAL_REF
      {  5.0 },              // PPPL ACCELERATION1
      { -0.1 },              // PPPL ACCELERATION2
      { -2.0 },              // PPPL ACCELERATION3
      {  1.0 },              // PPPL RATE2
      {  0.0 },              // PPPL RATE4
      {  1.0 },              // PPPL REF4
      {  0.1 } },            // PPPL DURATION4
}
#endif
;

// PPPL data description structure

CCPARS_PPPL_EXT struct CCpars   pppl_pars[]
#ifdef GLOBALS
= {// "Signal name"    type,         max_n_els, *enum,        *value,                    num_defaults      cyc_sel_step     flags
    { "INITIAL_REF",   PAR_FLOAT,            1,  NULL, { .f = &ccpars_pppl[0].initial_ref   }, 1, sizeof(struct CCpars_pppl), PARS_RW|PARS_REF },
    { "ACCELERATION1", PAR_FLOAT, FG_MAX_PPPLS,  NULL, { .f =  ccpars_pppl[0].acceleration1 }, 1, sizeof(struct CCpars_pppl), PARS_RW|PARS_REF },
    { "ACCELERATION2", PAR_FLOAT, FG_MAX_PPPLS,  NULL, { .f =  ccpars_pppl[0].acceleration2 }, 1, sizeof(struct CCpars_pppl), PARS_RW|PARS_REF },
    { "ACCELERATION3", PAR_FLOAT, FG_MAX_PPPLS,  NULL, { .f =  ccpars_pppl[0].acceleration3 }, 1, sizeof(struct CCpars_pppl), PARS_RW|PARS_REF },
    { "RATE2",         PAR_FLOAT, FG_MAX_PPPLS,  NULL, { .f =  ccpars_pppl[0].rate2         }, 1, sizeof(struct CCpars_pppl), PARS_RW|PARS_REF },
    { "RATE4",         PAR_FLOAT, FG_MAX_PPPLS,  NULL, { .f =  ccpars_pppl[0].rate4         }, 1, sizeof(struct CCpars_pppl), PARS_RW|PARS_REF },
    { "REF4",          PAR_FLOAT, FG_MAX_PPPLS,  NULL, { .f =  ccpars_pppl[0].ref4          }, 1, sizeof(struct CCpars_pppl), PARS_RW|PARS_REF },
    { "DURATION4",     PAR_FLOAT, FG_MAX_PPPLS,  NULL, { .f =  ccpars_pppl[0].duration4     }, 1, sizeof(struct CCpars_pppl), PARS_RW|PARS_REF },
    { NULL }
}
#endif
;

#endif
// EOF
