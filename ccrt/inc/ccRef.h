/*!
 * @file  ccrt/inc/ccRef.h
 *
 * @brief ccrt header file for ccRef.c
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

enum FG_errno      ccRefInitPLEP        (struct FG_meta *fg_meta, uint32_t cyc_sel);
enum FG_errno      ccRefInitRAMP        (struct FG_meta *fg_meta, uint32_t cyc_sel);
enum FG_errno      ccRefInitPPPL        (struct FG_meta *fg_meta, uint32_t cyc_sel);
enum FG_errno      ccRefInitTABLE       (struct FG_meta *fg_meta, uint32_t cyc_sel);
enum FG_errno      ccRefInitSTEPS       (struct FG_meta *fg_meta, uint32_t cyc_sel);
enum FG_errno      ccRefInitSQUARE      (struct FG_meta *fg_meta, uint32_t cyc_sel);
enum FG_errno      ccRefInitSINE        (struct FG_meta *fg_meta, uint32_t cyc_sel);
enum FG_errno      ccRefInitCOSINE      (struct FG_meta *fg_meta, uint32_t cyc_sel);
enum FG_errno      ccRefInitLTRIM       (struct FG_meta *fg_meta, uint32_t cyc_sel);
enum FG_errno      ccRefInitCTRIM       (struct FG_meta *fg_meta, uint32_t cyc_sel);
enum FG_errno      ccRefInitPULSE       (struct FG_meta *fg_meta, uint32_t cyc_sel);

// Reference functions structure

struct fgfunc
{
    enum cccmds_enum         cmd_idx;
    char                    *fg_pars;
    size_t                   size_of_pars;
    enum FG_errno           (*init_func)(struct FG_meta *FG_meta, uint32_t cyc_sel);
    enum FG_func_status      (*fgen_func)();
};

CCREF_EXT struct fgfunc funcs[]  // Must be in enum fg_types order (in ref.h)
#ifdef GLOBALS
= {
    {   0,          NULL,             0,                       NULL,            NULL           },
    {   CMD_PLEP,  (char *)&fg_plep,  sizeof(struct fg_plep),  ccRefInitPLEP,   fgPlepGen      },
    {   CMD_RAMP,  (char *)&fg_ramp,  sizeof(struct fg_ramp),  ccRefInitRAMP,   fgRampGen      },
    {   CMD_PPPL,  (char *)&fg_pppl,  sizeof(struct fg_pppl),  ccRefInitPPPL,   fgPpplGen      },
    {   CMD_TABLE, (char *)&fg_table, sizeof(struct fg_table), ccRefInitTABLE,  fgTableGen     },
    {   CMD_TEST,  (char *)&fg_test,  sizeof(struct fg_test),  ccRefInitSTEPS,  fgTestGen      },
    {   CMD_TEST,  (char *)&fg_test,  sizeof(struct fg_test),  ccRefInitSQUARE, fgTestGen      },
    {   CMD_TEST,  (char *)&fg_test,  sizeof(struct fg_test),  ccRefInitSINE,   fgTestGen      },
    {   CMD_TEST,  (char *)&fg_test,  sizeof(struct fg_test),  ccRefInitCOSINE, fgTestGen      },
    {   CMD_TRIM,  (char *)&fg_trim,  sizeof(struct fg_trim),  ccRefInitLTRIM,  fgTrimGen      },
    {   CMD_TRIM,  (char *)&fg_trim,  sizeof(struct fg_trim),  ccRefInitCTRIM,  fgTrimGen      },
    {   CMD_PULSE, (char *)&fg_pulse, sizeof(struct fg_pulse), ccRefInitPULSE,  fgPulseGen     },
}
#endif
;
#endif

// EOF
