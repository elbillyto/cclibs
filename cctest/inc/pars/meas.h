/*---------------------------------------------------------------------------------------------------------*\
  File:     cctest/inc/pars/meas.h                                                      Copyright CERN 2015

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

  Purpose:  Structure for the measurement parameters

  Authors:  Quentin.King@cern.ch
\*---------------------------------------------------------------------------------------------------------*/

#ifndef CCPARS_MEAS_H
#define CCPARS_MEAS_H

#include "ccPars.h"

// GLOBALS should be defined in the source file where global variables should be defined

#ifdef GLOBALS
#define CCPARS_MEAS_EXT
#else
#define CCPARS_MEAS_EXT extern
#endif

// Regulation measurement selector

CCPARS_MEAS_EXT struct CCpars_enum enum_reg_meas_select[]
#ifdef GLOBALS
= {
    { REG_MEAS_UNFILTERED,   "UNFILTERED"   },
    { REG_MEAS_FILTERED,     "FILTERED"     },
    { REG_MEAS_EXTRAPOLATED, "EXTRAPOLATED" },
    { 0,                      NULL          },
}
#endif
;

// Meas parameters structure

struct CCpars_meas
{
    enum REG_meas_select    b_reg_select;               // Field   measurement for regulation selector
    enum REG_meas_select    i_reg_select;               // Current measurement for regulation selector

    float                   b_delay_iters;              // Field   measurement delay in iterations
    float                   i_delay_iters;              // Current measurement delay in iterations
    float                   v_delay_iters;              // Voltage measurement delay in iterations

    uint32_t                b_fir_lengths[2];           // Field   measurement FIR filter lengths (in iterations)
    uint32_t                i_fir_lengths[2];           // Current measurement FIR filter lengths (in iterations)

    float                   b_sim_noise_pp;             // Simulated field   measurement peak-peak noise level
    float                   i_sim_noise_pp;             // Simulated current measurement peak-peak noise level
    float                   v_sim_noise_pp;             // Simulated voltage measurement peak-peak noise level

    float                   b_sim_quantization;         // Simulated field   measurement quantization
    float                   i_sim_quantization;         // Simulated current measurement quantization
    float                   v_sim_quantization;         // Simulated voltage measurement quantization

    uint32_t                sim_tone_period_iters;      // Simulated measurement tone period in iterations
    float                   b_sim_tone_pp;              // Simulated field   measurement tone peak-peak amplitude
    float                   i_sim_tone_pp;              // Simulated current measurement tone peak-peak amplitude

    float                   invalid_probability;        // Probablility of invalid measurements (0-1)
};

CCPARS_MEAS_EXT struct CCpars_meas ccpars_meas
#ifdef GLOBALS
= {//   Default value               Parameter
        REG_MEAS_EXTRAPOLATED,   // MEAS B_REG_SELECT
        REG_MEAS_EXTRAPOLATED,   // MEAS I_REG_SELECT
        1.3,                     // MEAS B_DELAY_ITERS
        1.3,                     // MEAS I_DELAY_ITERS
        1.3,                     // MEAS V_DELAY_ITERS
        { 0, 0 },                // MEAS B_FIR_LENGTHS
        { 0, 0 },                // MEAS I_FIR_LENGTHS
        0.0,                     // MEAS B_SIM_NOISE_PP
        0.0,                     // MEAS I_SIM_NOISE_PP
        0.0,                     // MEAS V_SIM_NOISE_PP
        0.0,                     // MEAS B_SIM_QUANTIZATION
        0.0,                     // MEAS I_SIM_QUANTIZATION
        0.0,                     // MEAS V_SIM_QUANTIZATION
        20,                      // MEAS SIM_TONE_PERIOD_ITERS
        0.0,                     // MEAS I_SIM_TONE_PP
        0.0,                     // MEAS B_SIM_TONE_PP
        0.0,                     // MEAS INVALID_PROBABILITY
}
#endif
;

// Meas parameters description structure

CCPARS_MEAS_EXT struct CCpars meas_pars[]
#ifdef GLOBALS
= {// "Signal name"             type,      max_n_els,*enum,                        *value,                    num_defaults,cyc_sel_step,flags
    { "B_REG_SELECT",           PAR_ENUM,      1,     enum_reg_meas_select, { .u = &ccpars_meas.b_reg_select          }, 1, 0, 0                 },
    { "I_REG_SELECT",           PAR_ENUM,      1,     enum_reg_meas_select, { .u = &ccpars_meas.i_reg_select          }, 1, 0, 0                 },
    { "B_DELAY_ITERS",          PAR_FLOAT,     1,     NULL,                 { .f = &ccpars_meas.b_delay_iters         }, 1, 0, 0                 },
    { "I_DELAY_ITERS",          PAR_FLOAT,     1,     NULL,                 { .f = &ccpars_meas.i_delay_iters         }, 1, 0, 0                 },
    { "V_DELAY_ITERS",          PAR_FLOAT,     1,     NULL,                 { .f = &ccpars_meas.v_delay_iters         }, 1, 0, 0                 },
    { "B_FIR_LENGTHS",          PAR_UNSIGNED,  2,     NULL,                 { .u =  ccpars_meas.b_fir_lengths         }, 2, 0, PARS_FIXED_LENGTH },
    { "I_FIR_LENGTHS",          PAR_UNSIGNED,  2,     NULL,                 { .u =  ccpars_meas.i_fir_lengths         }, 2, 0, PARS_FIXED_LENGTH },
    { "B_SIM_NOISE_PP",         PAR_FLOAT,     1,     NULL,                 { .f = &ccpars_meas.b_sim_noise_pp        }, 1, 0, 0                 },
    { "I_SIM_NOISE_PP",         PAR_FLOAT,     1,     NULL,                 { .f = &ccpars_meas.i_sim_noise_pp        }, 1, 0, 0                 },
    { "V_SIM_NOISE_PP",         PAR_FLOAT,     1,     NULL,                 { .f = &ccpars_meas.v_sim_noise_pp        }, 1, 0, 0                 },
    { "B_SIM_QUANTIZATION",     PAR_FLOAT,     1,     NULL,                 { .f = &ccpars_meas.b_sim_quantization    }, 1, 0, 0                 },
    { "I_SIM_QUANTIZATION",     PAR_FLOAT,     1,     NULL,                 { .f = &ccpars_meas.i_sim_quantization    }, 1, 0, 0                 },
    { "V_SIM_QUANTIZATION",     PAR_FLOAT,     1,     NULL,                 { .f = &ccpars_meas.v_sim_quantization    }, 1, 0, 0                 },
    { "SIM_TONE_PERIOD_ITERS",  PAR_UNSIGNED,  1,     NULL,                 { .u = &ccpars_meas.sim_tone_period_iters }, 1, 0, 0                 },
    { "B_SIM_TONE_PP",          PAR_FLOAT,     1,     NULL,                 { .f = &ccpars_meas.b_sim_tone_pp         }, 1, 0, 0                 },
    { "I_SIM_TONE_PP",          PAR_FLOAT,     1,     NULL,                 { .f = &ccpars_meas.i_sim_tone_pp         }, 1, 0, 0                 },
    { "INVALID_PROBABILITY",    PAR_FLOAT,     1,     NULL,                 { .f = &ccpars_meas.invalid_probability   }, 1, 0, 0                 },
    { NULL }
}
#endif
;

#endif
// EOF

