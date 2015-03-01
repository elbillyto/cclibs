/*!
 * @file  ccrt/inc/ccCmds.h
 *
 * @brief ccrt header file for ccCmds.c
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

#ifndef CCCMDS_H
#define CCCMDS_H

#include <stdint.h>
#include <stdbool.h>

// GLOBALS should be defined in the source file where global variables should be defined

#ifdef GLOBALS
#define CCCMDS_EXT
#else
#define CCCMDS_EXT extern
#endif

// Constants

#define CC_MAX_FILE_LINE_LEN  65536
#define CC_MAX_CMD_NAME_LEN   9             //  Current longest command name
#define CC_MAX_PAR_NAME_LEN   34            //  Current longest parameter name
#define CC_PROMPT             ">"

// Include libref header files

#include "libref.h"

// Include ccPars.h

#include "ccPars.h"

// Include libreg header files - required by cccmds[]

#include "libreg.h"

// Include cctest function data header files

#include "func/plep.h"
#include "func/pppl.h"
#include "func/pulse.h"
#include "func/ramp.h"
#include "func/table.h"
#include "func/test.h"
#include "func/trim.h"

// Include cctest parameter header files - required by cccmds[]

#include "pars/state.h"
#include "pars/global.h"
#include "pars/default.h"
#include "pars/direct.h"
#include "pars/limits.h"
#include "pars/load.h"
#include "pars/meas.h"
#include "pars/reg.h"
#include "pars/pc.h"
#include "pars/polswitch.h"
#include "pars/faults.h"
#include "pars/warnings.h"
#include "pars/prefunc.h"
#include "pars/postfunc.h"
#include "pars/ref.h"

// Function declarations

uint32_t ccCmdsPar   (uint32_t cmd_idx, char *remaining_line);
uint32_t ccCmdsHelp  (uint32_t cmd_idx, char *remaining_line);
uint32_t ccCmdsRead  (uint32_t cmd_idx, char *remaining_line);
uint32_t ccCmdsArm   (uint32_t cmd_idx, char *remaining_line);
uint32_t ccCmdsWait  (uint32_t cmd_idx, char *remaining_line);
uint32_t ccCmdsLog   (uint32_t cmd_idx, char *remaining_line);
uint32_t ccCmdsExit  (uint32_t cmd_idx, char *remaining_line);
uint32_t ccCmdsQuit  (uint32_t cmd_idx, char *remaining_line);

// Command indexes - the order of enum cccmds_enum must match the cmds[] array below

enum cccmds_enum
{
    /* Global parameters */
    CMD_GLOBAL,
    CMD_STATE,
    CMD_DEFAULT,
    CMD_DIRECT,
    CMD_LIMITS,
    CMD_LOAD,
    CMD_MEAS,
    CMD_BREG,
    CMD_IREG,
    CMD_PC,
    CMD_POLSWITCH,
    CMD_FAULTS,
    CMD_WARNINGS,
    CMD_PREFUNC,
    CMD_POSTFUNC,
    CMD_REF,
    /* Function parameters */
    CMD_PLEP,
    CMD_PPPL,
    CMD_PULSE,
    CMD_RAMP,
    CMD_TABLE,
    CMD_TEST,
    CMD_TRIM,
    /* Commands */
    CMD_HELP,
    CMD_READ,
    CMD_ARM,
    CMD_WAIT,
    CMD_LOG,
    CMD_EXIT,
    CMD_QUIT,

    N_CMDS
    };
    
    // Define array of commands

struct cccmds
{
    char                *name;
    uint32_t           (*cmd_func)(uint32_t cmd_idx, char *remaining_line);
    struct ccpars       *pars;
    char                *help_message;
    uint32_t             flags;
};

CCCMDS_EXT struct cccmds cmds[] // The order must match enum cccmds_enum (above)
#ifdef GLOBALS
= {
	// Global parameters
    { "GLOBAL",    ccCmdsPar  , global_pars   , "           Print or set GLOBAL parameter(s)"                       },
    { "STATE",     ccCmdsPar  , state_pars    , "           Print        STATE parameter(s)"                        },
    { "DEFAULT",   ccCmdsPar  , default_pars  , "           Print or set DEFAULT parameter(s)"                      },
    { "DIRECT",    ccCmdsPar  , direct_pars   , "           Print or set DIRECT parameter(s)"                       },
    { "LIMITS",    ccCmdsPar  , limits_pars   , "           Print or set LIMITS parameter(s)"                       },
    { "LOAD",      ccCmdsPar  , load_pars     , "           Print or set LOAD parameter(s)"                         },
    { "MEAS",      ccCmdsPar  , meas_pars     , "           Print or set MEAS parameter(s)"                         },
    { "BREG",      ccCmdsPar  , breg_pars     , "           Print or set BREG parameter(s)"                         },
    { "IREG",      ccCmdsPar  , ireg_pars     , "           Print or set IREG parameter(s)"                         },
    { "PC",        ccCmdsPar  , pc_pars       , "           Print or set PC parameter(s)"                           },
    { "POLSWITCH", ccCmdsPar  , polswitch_pars, "           Print or set POLSWITCH parameter(s)"                    },
    { "FAULTS",    ccCmdsPar  , faults_pars   , "           Print        FAULTS parameter(s)"                       },
    { "WARNINGS",  ccCmdsPar  , warnings_pars , "           Print        WARNINGS parameter(s)"                     },
    { "PREFUNC",   ccCmdsPar  , prefunc_pars  , "           Print or set PREFUNC parameter(s)"                      },
    { "POSTFUNC",  ccCmdsPar  , postfunc_pars , "           Print or set POSTFUNC parameter(s)"                     },
    { "REF",       ccCmdsPar  , ref_pars      , "           Print or set REF parameter(s)"                          },
    // Function parameters
    { "PLEP",      ccCmdsPar  , plep_pars     , "           Print or set PLEP function parameter(s)"                },
    { "PPPL",      ccCmdsPar  , pppl_pars     , "           Print or set PPPL function parameter(s)"                },
    { "PULSE",     ccCmdsPar  , pulse_pars    , "           Print or set PULSE function parameter(s)"               },
    { "RAMP",      ccCmdsPar  , ramp_pars     , "           Print or set RAMP function parameter(s)"                },
    { "TABLE",     ccCmdsPar  , table_pars    , "           Print or set TABLE function parameter(s)"               },
    { "TEST",      ccCmdsPar  , test_pars     , "           Print or set TEST function parameter(s)"                },
    { "TRIM",      ccCmdsPar  , trim_pars     , "           Print or set TRIM function parameter(s)"                },
    // Commands
    { "HELP",      ccCmdsHelp , NULL          , "           Print this help message"                                },
    { "READ",      ccCmdsRead , NULL          , "[filename] Read parameters from named file or from stdin"          },
    { "ARM",       ccCmdsArm  , NULL          , "[cyc_sel]  Arm reference function for cyc_sel"                     },
    { "WAIT",      ccCmdsWait , NULL          , "[seconds]  Wait specified time (default is 1s)"                    },
    { "LOG",       ccCmdsLog  , NULL          , "           Write log file with length GLOBAL LOG_DURATION"         },
    { "EXIT",      ccCmdsExit , NULL          , "           Exit from current file or quit when from stdin"         },
    { "QUIT",      ccCmdsQuit , NULL          , "           Quit program immediately"                               },
    { NULL }
}
#endif
;

#endif
// EOF
