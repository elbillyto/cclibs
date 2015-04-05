/*!
 * @file  ccrt/inc/pars/polswitch.h
 *
 * @brief ccrt POLSWITCH parameters
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

#ifndef CCPARS_POLSWITCH_H
#define CCPARS_POLSWITCH_H

#include "ccPars.h"

// GLOBALS should be defined in the source file where global variables should be defined

#ifdef GLOBALS
#define CCPARS_POLSWITCH_EXT
#else
#define CCPARS_POLSWITCH_EXT extern
#endif

// PolSwitch policies enum

enum polswitch_cmd
{
    POLSWITCH_CMD_AUTOMATIC,
    POLSWITCH_CMD_POSITIVE,
    POLSWITCH_CMD_NEGATIVE,
};

CCPARS_POLSWITCH_EXT struct CCpars_enum enum_polswitch_cmd[]
#ifdef GLOBALS
= { //                                            flags
    { POLSWITCH_CMD_AUTOMATIC,   "AUTOMATIC",     CC_ENUM_CYAN    },
    { POLSWITCH_CMD_POSITIVE,    "POSITIVE",      CC_ENUM_GREEN   },
    { POLSWITCH_CMD_NEGATIVE,    "NEGATIVE",      CC_ENUM_GREEN   },
    { 0,                          NULL                            },
}
#endif
;

// PolSwitch parameters structure

struct ccpars_polswitch
{
    float                       timeout;                // Polarity switch movement timeout (s), 0 = No switch
    float                       sim_delay;              // Polarity switch simulated movement delay (s)
    enum polswitch_cmd          command;                // Switch command when not automatic
};

CCPARS_POLSWITCH_EXT struct ccpars_polswitch ccpars_polswitch
#ifdef GLOBALS
= {// PolSwitch value            Parameter
    0.0,                      // POLSWITCH TIMEOUT
    0.0,                      // POLSWITCH DELAY
    POLSWITCH_CMD_AUTOMATIC,  // POLSWITCH COMMAND
}
#endif
;


// State parameter indexes

enum polswitch_pars_index_enum
{
    POLSWITCH_TIMEOUT   ,
    POLSWITCH_DELAY     ,
    POLSWITCH_COMMAND   ,
};

// PolSwitch parameters description structure

CCPARS_POLSWITCH_EXT struct CCpars polswitch_pars[]
#ifdef GLOBALS
= {// "Signal name"       type,  max_n_els, *enum,                       *value,             num_polswitchs,cyc_sel_step,flags
    { "TIMEOUT",          PAR_FLOAT, 1,      NULL,                  { .f = &ccpars_polswitch.timeout   }, 1,      0,     PARS_RW|PARS_CFG },
    { "SIM_DELAY",        PAR_FLOAT, 1,      NULL,                  { .f = &ccpars_polswitch.sim_delay }, 1,      0,     PARS_RW|PARS_CFG },
    { "COMMAND",          PAR_ENUM,  1,      enum_polswitch_cmd,    { .u = &ccpars_polswitch.command   }, 1,      0,     PARS_RW|PARS_CFG },
    { NULL }
}
#endif
;

#endif
// EOF
