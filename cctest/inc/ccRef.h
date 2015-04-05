/*---------------------------------------------------------------------------------------------------------*\
  File:     cctest/inc/ccRef.h                                                          Copyright CERN 2015

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
\*---------------------------------------------------------------------------------------------------------*/

#ifndef CCREF_H
#define CCREF_H

#include "ccCmds.h"

// GLOBALS should be defined in the source file where global variables should be defined

#ifdef GLOBALS
#define CCREF_EXT
#else
#define CCREF_EXT extern
#endif

// Function prototypes

enum FG_func_status ccRefDirectRT    (union  FG_pars *pars, FG_float func_time, float *ref);

enum FG_errno       ccRefInitPLEP    (struct FG_error *fg_error, uint32_t cyc_sel);
enum FG_errno       ccRefInitRAMP    (struct FG_error *fg_error, uint32_t cyc_sel);
enum FG_errno       ccRefInitPPPL    (struct FG_error *fg_error, uint32_t cyc_sel);
enum FG_errno       ccRefInitTABLE   (struct FG_error *fg_error, uint32_t cyc_sel);
enum FG_errno       ccRefInitSTEPS   (struct FG_error *fg_error, uint32_t cyc_sel);
enum FG_errno       ccRefInitSQUARE  (struct FG_error *fg_error, uint32_t cyc_sel);
enum FG_errno       ccRefInitSINE    (struct FG_error *fg_error, uint32_t cyc_sel);
enum FG_errno       ccRefInitCOSINE  (struct FG_error *fg_error, uint32_t cyc_sel);
enum FG_errno       ccRefInitLTRIM   (struct FG_error *fg_error, uint32_t cyc_sel);
enum FG_errno       ccRefInitCTRIM   (struct FG_error *fg_error, uint32_t cyc_sel);
enum FG_errno       ccRefInitPULSE   (struct FG_error *fg_error, uint32_t cyc_sel);

// Reference functions structure

struct fgfunc
{
    enum cccmds_enum         cmd_idx;
    enum FG_errno           (*init_func)(struct FG_error *fg_error, uint32_t cyc_sel);
    FG_FuncRT                fg_func;
};

CCREF_EXT struct fgfunc funcs[]  // Must be in enum fg_types order (in ref.h)
#ifdef GLOBALS
= {
    {   0,         NULL,            NULL           },
    {   CMD_TABLE, ccRefInitTABLE,  ccRefDirectRT },
    {   CMD_PLEP,  ccRefInitPLEP,   fgPlepRT      },
    {   CMD_RAMP,  ccRefInitRAMP,   fgRampRT      },
    {   CMD_PPPL,  ccRefInitPPPL,   fgPpplRT      },
    {   CMD_TABLE, ccRefInitTABLE,  fgTableRT     },
    {   CMD_TEST,  ccRefInitSTEPS,  fgTestRT      },
    {   CMD_TEST,  ccRefInitSQUARE, fgTestRT      },
    {   CMD_TEST,  ccRefInitSINE,   fgTestRT      },
    {   CMD_TEST,  ccRefInitCOSINE, fgTestRT      },
    {   CMD_TRIM,  ccRefInitLTRIM,  fgTrimRT      },
    {   CMD_TRIM,  ccRefInitCTRIM,  fgTrimRT      },
    {   CMD_PULSE, ccRefInitPULSE,  fgPulseRT     },
}
#endif
;
#endif

// EOF
