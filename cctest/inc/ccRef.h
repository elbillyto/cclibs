/*---------------------------------------------------------------------------------------------------------*\
  File:     cctest/inc/ccRef.h                                                          Copyright CERN 2014

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

  Purpose:  Header file for FG library test program reference generation functions

  Authors:  Quentin.King@cern.ch
\*---------------------------------------------------------------------------------------------------------*/

#ifndef CCREF_H
#define CCREF_H

#include "ccCmds.h"

// Include fg and reg library header files

#include "libfg.h"
#include "libfg/plep.h"
#include "libfg/ramp.h"
#include "libfg/pppl.h"
#include "libfg/table.h"
#include "libfg/test.h"
#include "libfg/trim.h"

// Include start function header

#include "func/start.h"

// GLOBALS should be defined in the source file where global variables should be defined

#ifdef GLOBALS
#define CCREF_EXT
#else
#define CCREF_EXT extern
#endif

// Function prototypes

bool            ccRefDirectGen          (struct fg_table_pars *pars, const double *time, float *ref);

uint32_t        ccRefInitSTART          (struct fg_meta *fg_meta);
uint32_t        ccRefInitDIRECT         (struct fg_meta *fg_meta);
uint32_t        ccRefInitPLEP           (struct fg_meta *fg_meta);
uint32_t        ccRefInitRAMP           (struct fg_meta *fg_meta);
uint32_t        ccRefInitPPPL           (struct fg_meta *fg_meta);
uint32_t        ccRefInitTABLE          (struct fg_meta *fg_meta);
uint32_t        ccRefInitSTEPS          (struct fg_meta *fg_meta);
uint32_t        ccRefInitSQUARE         (struct fg_meta *fg_meta);
uint32_t        ccRefInitSINE           (struct fg_meta *fg_meta);
uint32_t        ccRefInitCOSINE         (struct fg_meta *fg_meta);
uint32_t        ccRefInitLTRIM          (struct fg_meta *fg_meta);
uint32_t        ccRefInitCTRIM          (struct fg_meta *fg_meta);

// Reference functions structure

struct fgfunc
{
    enum cccmds_enum         cmd_idx;
    void                    *fg_pars;
    enum fg_error           (*init_func)(struct fg_meta *fg_meta);
    bool                    (*fgen_func)();
};

CCREF_EXT struct fgfunc funcs[]  // Must be in enum fg_types order (in global.h)!
#ifdef GLOBALS
= {
    {   CMD_START, &ccpars_start.pars, ccRefInitSTART,  fgRampGen      },
    {   CMD_TABLE, &ccpars_table.pars, ccRefInitTABLE,  ccRefDirectGen },
    {   CMD_PLEP,  &ccpars_plep.pars,  ccRefInitPLEP,   fgPlepGen      },
    {   CMD_RAMP,  &ccpars_ramp.pars,  ccRefInitRAMP,   fgRampGen      },
    {   CMD_PPPL,  &ccpars_pppl.pars,  ccRefInitPPPL,   fgPpplGen      },
    {   CMD_TABLE, &ccpars_table.pars, ccRefInitTABLE,  fgTableGen     },
    {   CMD_TEST,  &ccpars_test.pars,  ccRefInitSTEPS,  fgTestGen      },
    {   CMD_TEST,  &ccpars_test.pars,  ccRefInitSQUARE, fgTestGen      },
    {   CMD_TEST,  &ccpars_test.pars,  ccRefInitSINE,   fgTestGen      },
    {   CMD_TEST,  &ccpars_test.pars,  ccRefInitCOSINE, fgTestGen      },
    {   CMD_TRIM,  &ccpars_trim.pars,  ccRefInitLTRIM,  fgTrimGen      },
    {   CMD_TRIM,  &ccpars_trim.pars,  ccRefInitCTRIM,  fgTrimGen      },
}
#endif
;
#endif
// EOF
