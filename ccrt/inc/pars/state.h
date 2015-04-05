/*!
 * @file  ccrt/inc/pars/state.h
 *
 * @brief ccrt STATE parameters
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

#ifndef CCPARS_STATE_H
#define CCPARS_STATE_H

#include "ccPars.h"

// GLOBALS should be defined in the source file where global variables should be defined

#ifdef GLOBALS
#define CCPARS_STATE_EXT
#else
#define CCPARS_STATE_EXT extern
#endif

// Power converter state enum

enum state_pc
{
    PC_OFF,
    PC_ON,
    PC_FAULT,
};

CCPARS_STATE_EXT struct CCpars_enum enum_state_pc[]
#ifdef GLOBALS
= { //                               flags
    {  PC_OFF,       "OFF",          0                                  },
    {  PC_ON,        "ON",           CC_ENUM_GREEN                      },
    {  PC_FAULT,     "FAULT",        CC_ENUM_RED|CC_ENUM_READ_ONLY      },
    { 0,              NULL                                              },
}
#endif
;

// Reference state enum

CCPARS_STATE_EXT struct CCpars_enum enum_state_ref[]
#ifdef GLOBALS
= { //                                          flags
    { REF_OFF,              "OFF",              0                                },
    { REF_POL_SWITCHING,    "POL_SWITCHING",    CC_ENUM_CYAN|CC_ENUM_READ_ONLY   },
    { REF_TO_OFF,           "TO_OFF",           CC_ENUM_CYAN|CC_ENUM_READ_ONLY   },
    { REF_TO_STANDBY,       "TO_STANDBY",       CC_ENUM_CYAN|CC_ENUM_READ_ONLY   },
    { REF_TO_CYCLING,       "TO_CYCLING",       CC_ENUM_CYAN|CC_ENUM_READ_ONLY   },
    { REF_TO_IDLE,          "TO_IDLE",          CC_ENUM_CYAN|CC_ENUM_READ_ONLY   },
    { REF_DIRECT,           "DIRECT",           CC_ENUM_GREEN                    },
    { REF_ON_STANDBY,       "ON_STANDBY",       CC_ENUM_GREEN                    },
    { REF_CYCLING,          "CYCLING",          CC_ENUM_GREEN                    },
    { REF_ECONOMY,          "ECONOMY",          CC_ENUM_READ_ONLY                },
    { REF_IDLE,             "IDLE",             CC_ENUM_GREEN                    },
    { REF_ARMED,            "ARMED",            CC_ENUM_CYAN|CC_ENUM_READ_ONLY   },
    { REF_RUNNING,          "RUNNING",          CC_ENUM_CYAN|CC_ENUM_READ_ONLY   },
    { REF_PAUSED,           "PAUSED",           CC_ENUM_CYAN|CC_ENUM_READ_ONLY   },
    { 0,                     NULL                                                },
}
#endif
;

// Polarity switch state enum

enum state_polswitch
{
    POLSWITCH_NONE,
    POLSWITCH_POSITIVE,
    POLSWITCH_NEGATIVE,
    POLSWITCH_MOVING,
    POLSWITCH_FAULT,
};

CCPARS_STATE_EXT struct CCpars_enum enum_state_polswitch[]
#ifdef GLOBALS
= { //                                          flags
    {  POLSWITCH_NONE,       "NONE",            0                  },
    {  POLSWITCH_POSITIVE,   "POSITIVE",        CC_ENUM_GREEN      },
    {  POLSWITCH_NEGATIVE,   "NEGATIVE",        CC_ENUM_GREEN      },
    {  POLSWITCH_MOVING,     "MOVING",          CC_ENUM_YELLOW     },
    {  POLSWITCH_FAULT,      "FAULT",           CC_ENUM_RED        },
    { 0,                      NULL                                 },
}
#endif
;

// State parameters structure

struct ccpars_state
{
    enum state_pc           pc;                     // Power converter state (ON|OFF|FAULT)
    enum ref_state          ref;                    // Reference state (from libref.h)
    enum state_polswitch    polswitch;              // Polarity switch state
};

CCPARS_STATE_EXT struct ccpars_state ccpars_state;

// State parameter indexes

enum state_pars_index_enum
{
    STATE_PC               ,
    STATE_REF              ,
    STATE_POLSWITCH        ,
};

// State parameters description structure

CCPARS_STATE_EXT struct CCpars state_pars[]
#ifdef GLOBALS
= {// "Signal name" type,  max_n_els, *enum,                        *value,           num_states,cyc_sel_step,flags
    { "PC",         PAR_ENUM,  1,      enum_state_pc,        { .u = &ccpars_state.pc        }, 1,      0,     PARS_RO },
    { "REF",        PAR_ENUM,  1,      enum_state_ref,       { .u = &ccpars_state.ref       }, 1,      0,     PARS_RO },
    { "POLSWITCH",  PAR_ENUM,  1,      enum_state_polswitch, { .u = &ccpars_state.polswitch }, 1,      0,     PARS_RO },
    { NULL }
}
#endif
;

#endif
// EOF

