/*---------------------------------------------------------------------------------------------------------*\
  File:     cctest/inc/pars/pc.h                                                        Copyright CERN 2015

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

  Purpose:  Structure for the power converter mnodel parameters

  Authors:  Quentin.King@cern.ch
\*---------------------------------------------------------------------------------------------------------*/

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
= {
    { REG_VOLTAGE_REF,    "VOLTAGE" },
    { REG_CURRENT_REF,    "CURRENT" },
    { 0,                   NULL     },
}
#endif
;

// Power converter parameters structure

struct CCpars_pc
{
    enum REG_actuation          actuation;              // Power converter actuation (VOLTAGE REF or CURRENT REF)
    float                       act_delay_iters;        // Power converter control delay in iterations
    float                       sim_bandwidth;          // Simulated power converter (voltage source or current source) second order bandwidth
    float                       sim_z;                  // Simulated second order damping factor
    float                       sim_tau_zero;           // Simulated second order time constant of zero (0 if not required)
    struct REG_sim_pc_pars      sim_pc_pars;            // Simulated power converter third order model to use if sim_bandwidth is zero
    float                       sim_quantization;       // Simulated power converter actuation quantisation
    float                       sim_noise_pp;           // Simulated noise peak-peak amplitude
    uint32_t                    sim_tone_period_iters;  // Simulated tone period in iterations
    float                       sim_tone_pp;            // Simulated tone peak-peak amplitude
};

CCPARS_PC_EXT struct CCpars_pc ccpars_pc
#ifdef GLOBALS
= {//   Default value               Parameter
        REG_VOLTAGE_REF,         // PC ACTUATION
        1.0,                     // PC ACT_DELAY_ITERS
        200.0,                   // PC SIM_BANDWIDTH
        0.9,                     // PC SIM_Z
        0.0,                     // PC SIM_TAU_ZERO
        {  { 1.0 },              // PC SIM_NUM
           { 1.0 }  },           // PC SIM_DEN
        0.0,                     // PC SIM_QUANTIZATION
        0.0,                     // PC SIM_NOISE_PP
        33,                      // PC SIM_TONE_PERIOD_ITERS
        0.0,                     // PC SIM_TONE_PP
}
#endif
;

// Power converter parameters description structure

CCPARS_PC_EXT struct CCpars pc_pars[]
#ifdef GLOBALS
= {// "Signal name"            type,         max_n_els,            *enum,                      *value,                             num_defaults,    cyc_sel_step, flags
    { "ACTUATION",             PAR_ENUM,     1,                     enum_reg_actuation, { .u = &ccpars_pc.actuation             }, 1,                     0, 0                 },
    { "ACT_DELAY_ITERS",       PAR_FLOAT,    1,                     NULL,               { .f = &ccpars_pc.act_delay_iters       }, 1,                     0, 0                 },
    { "SIM_BANDWIDTH",         PAR_FLOAT,    1,                     NULL,               { .f = &ccpars_pc.sim_bandwidth         }, 1,                     0, 0                 },
    { "SIM_Z",                 PAR_FLOAT,    1,                     NULL,               { .f = &ccpars_pc.sim_z                 }, 1,                     0, 0                 },
    { "SIM_TAU_ZERO",          PAR_FLOAT,    1,                     NULL,               { .f = &ccpars_pc.sim_tau_zero          }, 1,                     0, 0                 },
    { "SIM_NUM",               PAR_FLOAT,    REG_NUM_PC_SIM_COEFFS, NULL,               { .f =  ccpars_pc.sim_pc_pars.num       }, REG_NUM_PC_SIM_COEFFS, 0, PARS_FIXED_LENGTH },
    { "SIM_DEN",               PAR_FLOAT,    REG_NUM_PC_SIM_COEFFS, NULL,               { .f =  ccpars_pc.sim_pc_pars.den       }, REG_NUM_PC_SIM_COEFFS, 0, PARS_FIXED_LENGTH },
    { "SIM_QUANTIZATION",      PAR_FLOAT,    1,                     NULL,               { .f = &ccpars_pc.sim_quantization      }, 1,                     0, 0                 },
    { "SIM_NOISE_PP",          PAR_FLOAT,    1,                     NULL,               { .f = &ccpars_pc.sim_noise_pp          }, 1,                     0, 0                 },
    { "SIM_TONE_PERIOD_ITERS", PAR_UNSIGNED, 1,                     NULL,               { .u = &ccpars_pc.sim_tone_period_iters }, 1,                     0, 0                 },
    { "SIM_TONE_PP",           PAR_FLOAT,    1,                     NULL,               { .f = &ccpars_pc.sim_tone_pp           }, 1,                     0, 0                 },
    { NULL }
}
#endif
;

#endif
// EOF

