/*!
 * @file  ccrt/inc/pars/pc.h
 *
 * @brief ccrt PC parameters
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

#ifndef CCPARS_PC_H
#define CCPARS_PC_H

#include "ccCmds.h"

// GLOBALS is defined in source file where global variables should be defined

#ifdef GLOBALS
#define CCPARS_PC_EXT
#else
#define CCPARS_PC_EXT extern
#endif

// Regulation actuation enum constants come from libreg

CCPARS_PC_EXT struct CCpars_enum enum_reg_actuation[]
#ifdef GLOBALS
= { //                                   flags
    { REG_VOLTAGE_REF,    "VOLTAGE",     CC_ENUM_GREEN     },
    { REG_CURRENT_REF,    "CURRENT",     CC_ENUM_YELLOW    },
    { 0,                   NULL                            },
}
#endif
;

// Power converter parameters structure

struct CCpars_pc
{
    enum state_pc               state;              // Power converter state control (ON, OFF)
    enum ref_state              ref;                // Reference state control
    enum REG_actuation          actuation;          // Power converter actuation (VOLTAGE REF or CURRENT REF)
    float                       act_delay_iters;    // Power converter control delay in iterations
    float                       bandwidth;          // Power converter (voltage source or current source) second order bandwidth
    float                       z;                  // Second order damping factor
    float                       tau_zero;           // Second order time constant of zero (0 if not required)
    struct REG_sim_pc_pars      sim_pc_pars;        // Power converter third order model if bandwidth is zero
    float                       sim_quantization;   // Simulated actuation quantization (V or A)
    float                       sim_ripple;         // Simulated peak-peak ripple
};

CCPARS_PC_EXT struct CCpars_pc ccpars_pc
#ifdef GLOBALS
= {//   Default value               Parameter
        PC_OFF,                  // PC STATE
        REF_OFF,                 // PC REF
        REG_VOLTAGE_REF,         // PC ACTUATION
        1.0,                     // PC ACT_DELAY_ITERS
        200.0,                   // PC BANDWIDTH
        0.9,                     // PC Z
        0.0,                     // PC TAU_ZERO
        {  { 1.0 },              // PC SIM_NUM
           { 1.0 }  },           // PC SIM_DEN
        0.0,                     // PC SIM_QUANTIZATION
        0.0,                     // PC SIM_RIPPLE
}
#endif
;

// PC parameters indexes

enum pc_pars_index_enum
{
    PC_STATE               ,
    PC_REF                 ,
    PC_ACTUATION           ,
    PC_ACT_DELAY_ITERS     ,
    PC_BANDWIDTH           ,
    PC_Z                   ,
    PC_TAU_ZERO            ,
    PC_SIM_NUM             ,
    PC_SIM_DEN             ,
    PC_SIM_QUANTIZATION    ,
    PC_SIM_RIPPLE          ,
};

// Power converter parameters description structure

CCPARS_PC_EXT struct CCpars pc_pars[]
#ifdef GLOBALS
= {// "Signal name"       type,      max_n_els,            *enum,                      *value,                       num_defaults,    cyc_sel_step, flags
    { "STATE",            PAR_ENUM,  1,                     enum_state_pc,      { .u = &CCpars_pc.state            }, 1,                     0, PARS_RW                               },
    { "REF",              PAR_ENUM,  1,                     enum_state_ref,     { .u = &CCpars_pc.ref              }, 1,                     0, PARS_RW                               },
    { "ACTUATION",        PAR_ENUM,  1,                     enum_reg_actuation, { .u = &CCpars_pc.actuation        }, 1,                     0, PARS_RW|PARS_REG|PARS_CFG             },
    { "ACT_DELAY_ITERS",  PAR_FLOAT, 1,                     NULL,               { .f = &CCpars_pc.act_delay_iters  }, 1,                     0, PARS_RW|PARS_REG|PARS_CFG             },
    { "BANDWIDTH",        PAR_FLOAT, 1,                     NULL,               { .f = &CCpars_pc.bandwidth        }, 1,                     0, PARS_RW|PARS_REG|PARS_CFG             },
    { "Z",                PAR_FLOAT, 1,                     NULL,               { .f = &CCpars_pc.z                }, 1,                     0, PARS_RW|PARS_REG|PARS_CFG             },
    { "TAU_ZERO",         PAR_FLOAT, 1,                     NULL,               { .f = &CCpars_pc.tau_zero         }, 1,                     0, PARS_RW|PARS_REG|PARS_CFG             },
    { "SIM_NUM",          PAR_FLOAT, REG_NUM_PC_SIM_COEFFS, NULL,               { .f =  CCpars_pc.sim_pc_pars.num  }, REG_NUM_PC_SIM_COEFFS, 0, PARS_RW|PARS_REG|PARS_CFG|PARS_FIXLEN },
    { "SIM_DEN",          PAR_FLOAT, REG_NUM_PC_SIM_COEFFS, NULL,               { .f =  CCpars_pc.sim_pc_pars.den  }, REG_NUM_PC_SIM_COEFFS, 0, PARS_RW|PARS_REG|PARS_CFG|PARS_FIXLEN },
    { "SIM_QUANTIZATION", PAR_FLOAT, 1,                     NULL,               { .f = &CCpars_pc.sim_quantization }, 1,                     0, PARS_RW|PARS_REG|PARS_CFG             },
    { "SIM_RIPPLE",       PAR_FLOAT, 1,                     NULL,               { .f = &CCpars_pc.sim_ripple       }, 1,                     0, PARS_RW|PARS_REG|PARS_CFG             },
    { NULL }
}
#endif
;

#endif
// EOF

