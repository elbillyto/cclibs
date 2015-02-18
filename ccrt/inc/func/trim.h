/*!
 * @file  ccrt/inc/func/trim.h
 *
 * @brief ccrt TRIM function parameters
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

#ifndef CCPARS_TRIM_H
#define CCPARS_TRIM_H

#include "ccRt.h"
#include "ccPars.h"
#include "libfg/trim.h"

// GLOBALS is defined in source file where global variables should be defined

#ifdef GLOBALS
#define CCPARS_TRIM_EXT
#else
#define CCPARS_TRIM_EXT extern
#endif

// Libfg TRIM parameter structures

CCPARS_TRIM_EXT struct fg_trim fg_trim[CC_NUM_CYC_SELS];

// Trim parameters structure

struct ccpars_trim
{
    // cctest TRIM parameters

    float                       initial_ref;                    // Initial reference
    enum fg_trim_type           type;                           // Type of trim
    float                       duration;                       // Time duration
    float                       final_ref;                      // Final reference
};

CCPARS_TRIM_EXT struct ccpars_trim ccpars_trim[CC_NUM_CYC_SELS]
#ifdef GLOBALS
= {// Default value                Parameter
    {   0.0,                    // TRIM INITIAL_REF
        FG_TRIM_LINEAR,         // Overwritten by init function (LTRIM or CTRIM)
        1.0,                    // TRIM DURATION
        1.0              },     // TRIM FINAL
}
#endif
;

// Trim data description structure

CCPARS_TRIM_EXT struct ccpars   trim_pars[]
#ifdef GLOBALS
= {// "Signal name"  type,    max_n_els,*enum,        *value,                  num_defaults      cyc_sel_step     flags
    { "INITIAL_REF", PAR_FLOAT,   1,     NULL, { .f = &ccpars_trim[0].initial_ref }, 1, sizeof(struct ccpars_trim), PARS_RW|PARS_REF },
    { "FINAL_REF",   PAR_FLOAT,   1,     NULL, { .f = &ccpars_trim[0].final_ref   }, 1, sizeof(struct ccpars_trim), PARS_RW|PARS_REF },
    { "DURATION",    PAR_FLOAT,   1,     NULL, { .f = &ccpars_trim[0].duration    }, 1, sizeof(struct ccpars_trim), PARS_RW|PARS_REF },
    { NULL }
}
#endif
;

#endif
// EOF
