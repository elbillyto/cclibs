/*!
 * @file  ccrt/inc/pars/meas.h
 *
 * @brief ccrt MEAS parameters
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

#ifndef CCPARS_MEAS_H
#define CCPARS_MEAS_H

#include "ccPars.h"
#include "pars/reg.h"

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

    uint32_t                tone_half_period_iters;     // Simulated measurement tone half-period in iterations
    float                   b_sim_tone_amp;             // Simulated field   measurement tone amplitude
    float                   i_sim_tone_amp;             // Simulated current measurement tone amplitude

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
        10,                      // MEAS TONE_HALF_PERIOD_ITERS
        0.0,                     // MEAS I_SIM_TONE_AMP
        0.0,                     // MEAS B_SIM_TONE_AMP
        0.0,                     // MEAS INVALID_PROBABILITY
}
#endif
;

// Meas parameters description structure

CCPARS_MEAS_EXT struct CCpars meas_pars[]
#ifdef GLOBALS
= {// "Signal name"             type,      max_n_els,*enum,                        *value,                                       num_defaults,cyc_sel_step,flags
    { "B_REG_SELECT",           PAR_ENUM,      1,     enum_reg_meas_select, { .u = &CCpars_meas.b_reg_select                             }, 1,      0,     PARS_RW|PARS_CFG|PARS_REG             },
    { "I_REG_SELECT",           PAR_ENUM,      1,     enum_reg_meas_select, { .u = &CCpars_meas.i_reg_select                             }, 1,      0,     PARS_RW|PARS_CFG|PARS_REG             },
    { "B_DELAY_ITERS",          PAR_FLOAT,     1,     NULL,                 { .f = &CCpars_meas.b_delay_iters                            }, 1,      0,     PARS_RW|PARS_CFG|PARS_REG             },
    { "I_DELAY_ITERS",          PAR_FLOAT,     1,     NULL,                 { .f = &CCpars_meas.i_delay_iters                            }, 1,      0,     PARS_RW|PARS_CFG|PARS_REG             },
    { "V_DELAY_ITERS",          PAR_FLOAT,     1,     NULL,                 { .f = &CCpars_meas.v_delay_iters                            }, 1,      0,     PARS_RW|PARS_CFG|PARS_REG             },
    { "B_FIR_LENGTHS",          PAR_UNSIGNED,  2,     NULL,                 { .u =  CCpars_meas.b_fir_lengths                            }, 2,      0,     PARS_RW|PARS_CFG|PARS_REG|PARS_FIXLEN },
    { "I_FIR_LENGTHS",          PAR_UNSIGNED,  2,     NULL,                 { .u =  CCpars_meas.i_fir_lengths                            }, 2,      0,     PARS_RW|PARS_CFG|PARS_REG|PARS_FIXLEN },
    { "B_SIM_NOISE_PP",         PAR_FLOAT,     1,     NULL,                 { .f = &CCpars_meas.b_sim_noise_pp                           }, 1,      0,     PARS_RW|PARS_CFG|PARS_REG             },
    { "I_SIM_NOISE_PP",         PAR_FLOAT,     1,     NULL,                 { .f = &CCpars_meas.i_sim_noise_pp                           }, 1,      0,     PARS_RW|PARS_CFG|PARS_REG             },
    { "V_SIM_NOISE_PP",         PAR_FLOAT,     1,     NULL,                 { .f = &CCpars_meas.v_sim_noise_pp                           }, 1,      0,     PARS_RW|PARS_CFG|PARS_REG             },
    { "B_SIM_QUANTIZATION",     PAR_FLOAT,     1,     NULL,                 { .f = &CCpars_meas.b_sim_quantization                       }, 1,      0,     PARS_RW|PARS_CFG|PARS_REG             },
    { "I_SIM_QUANTIZATION",     PAR_FLOAT,     1,     NULL,                 { .f = &CCpars_meas.i_sim_quantization                       }, 1,      0,     PARS_RW|PARS_CFG|PARS_REG             },
    { "V_SIM_QUANTIZATION",     PAR_FLOAT,     1,     NULL,                 { .f = &CCpars_meas.v_sim_quantization                       }, 1,      0,     PARS_RW|PARS_CFG|PARS_REG             },
    { "TONE_HALF_PERIOD_ITERS", PAR_UNSIGNED,  1,     NULL,                 { .u = &CCpars_meas.tone_half_period_iters                   }, 1,      0,     PARS_RW|PARS_CFG|PARS_REG             },
    { "B_SIM_TONE_AMP",         PAR_FLOAT,     1,     NULL,                 { .f = &CCpars_meas.b_sim_tone_amp                           }, 1,      0,     PARS_RW|PARS_CFG|PARS_REG             },
    { "I_SIM_TONE_AMP",         PAR_FLOAT,     1,     NULL,                 { .f = &CCpars_meas.i_sim_tone_amp                           }, 1,      0,     PARS_RW|PARS_CFG|PARS_REG             },
    { "INVALID_PROBABILITY",    PAR_FLOAT,     1,     NULL,                 { .f = &CCpars_meas.invalid_probability                      }, 1,      0,     PARS_RW|PARS_CFG                      },
    { "B_INVALID_COUNTER",      PAR_UNSIGNED,  1,     NULL,                 { .u = (uint32_t *)&regMgrVar(reg_mgr,MEAS_B_INVALID_COUNTER)}, 1,      0,     PARS_RO                               },
    { "I_INVALID_COUNTER",      PAR_UNSIGNED,  1,     NULL,                 { .u = (uint32_t *)&regMgrVar(reg_mgr,MEAS_I_INVALID_COUNTER)}, 1,      0,     PARS_RO                               },
    { "V_INVALID_COUNTER",      PAR_UNSIGNED,  1,     NULL,                 { .u = (uint32_t *)&regMgrVar(reg_mgr,MEAS_V_INVALID_COUNTER)}, 1,      0,     PARS_RO                               },
    { NULL }
}
#endif
;

#endif
// EOF

