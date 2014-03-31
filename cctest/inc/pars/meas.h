/*---------------------------------------------------------------------------------------------------------*\
  File:     pars/meas.h                                                                 Copyright CERN 2014

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

  Purpose:  Structure for the meas parameters group

  Authors:  Quentin.King@cern.ch
\*---------------------------------------------------------------------------------------------------------*/

#ifndef CCPARS_MEAS_H
#define CCPARS_MEAS_H

#include "ccpars.h"

// GLOBALS should be defined in the source file where global variables should be defined

#ifdef GLOBALS
#define CCPARS_MEAS_EXT
#else
#define CCPARS_MEAS_EXT extern
#endif

// Meas parameters structure

struct ccpars_meas
{
    // Meas parameters

    float                   v_delay_iters;              // Voltage measurement delay in iterations
    float                   i_delay_iters;              // Current measurement delay in iterations
    float                   b_delay_iters;              // Field   measurement delay in iterations

    uint32_t                i_extrapolate;              // Extrapolate current measurement for regulation
    uint32_t                b_extrapolate;              // Extrapolate field   measurement for regulation

    uint32_t                i_fir_lengths[2];           // Current measurement FIR filter lengths (in iterations)
    uint32_t                b_fir_lengths[2];           // Field   measurement FIR filter lengths (in iterations)

    float                   v_sim_noise_pp;             // Simulated voltage measurement peak-peak noise level
    float                   i_sim_noise_pp;             // Simulated current measurement peak-peak noise level
    float                   b_sim_noise_pp;             // Simulated field   measurement peak-peak noise level

    uint32_t                tone_half_period_iters;     // Simulated measurement tone half-period in iterations
    float                   i_sim_tone_amp;             // Simulated current measurement tone amplitude
    float                   b_sim_tone_amp;             // Simulated field   measurement tone amplitude
};

CCPARS_MEAS_EXT struct ccpars_meas ccpars_meas
#ifdef GLOBALS
= {
    1.3,                                      // V_DELAY_ITERS
    1.3,                                      // I_DELAY_ITERS
    1.3,                                      // B_DELAY_ITERS
    CC_DISABLED,                              // I_EXTRAPOLATE
    CC_DISABLED,                              // B_EXTRAPOLATE
    { 1.0, 1.0 },                             // I_FIR_LENGTHS
    { 1.0, 1.0 },                             // B_FIR_LENGTHS
    0.0,                                      // I_SIM_NOISE_PP
    0.0,                                      // I_SIM_NOISE_PP
    0.0,                                      // B_SIM_NOISE_PP
    100,                                      // TONE_HALF_PERIOD_ITERS
    0.0,                                      // I_SIM_TONE_AMP
    0.0,                                      // B_SIM_TONE_AMP
}
#endif
;

// Meas parameters description structure

CCPARS_MEAS_EXT struct ccpars meas_pars_list[]
#ifdef GLOBALS
= {// "Signal name"            TYPE,max_vals,min_vals,*enum,          *value,                               num_defaults
    { "V_DELAY_ITERS",         PAR_FLOAT,    1, 0, NULL,            { .f = &ccpars_meas.v_delay_iters          }, 1 },
    { "I_DELAY_ITERS",         PAR_FLOAT,    1, 0, NULL,            { .f = &ccpars_meas.i_delay_iters          }, 1 },
    { "B_DELAY_ITERS",         PAR_FLOAT,    1, 0, NULL,            { .f = &ccpars_meas.b_delay_iters          }, 1 },
    { "I_EXTRAPOLATE",         PAR_ENUM,     1, 0, enabled_disabled,{ .i = &ccpars_meas.i_extrapolate          }, 1 },
    { "I_FIR_LENGTHS",         PAR_FLOAT,    2, 0, NULL,            { .f =  ccpars_meas.i_fir_lengths          }, 2 },
    { "B_FIR_LENGTHS",         PAR_FLOAT,    2, 0, NULL,            { .f =  ccpars_meas.b_fir_lengths          }, 2 },
    { "V_SIM_NOISE_PP",        PAR_FLOAT,    1, 0, NULL,            { .f = &ccpars_meas.v_sim_noise_pp         }, 1 },
    { "I_SIM_NOISE_PP",        PAR_FLOAT,    1, 0, NULL,            { .f = &ccpars_meas.i_sim_noise_pp         }, 1 },
    { "B_SIM_NOISE_PP",        PAR_FLOAT,    1, 0, NULL,            { .f = &ccpars_meas.b_sim_noise_pp         }, 1 },
    { "TONE_HALF_PERIOD_ITERS",PAR_UNSIGNED, 1, 0, NULL,            { .i = &ccpars_meas.tone_half_period_iters }, 1 },
    { "I_SIM_TONE_AMP",        PAR_FLOAT,    1, 0, NULL,            { .f = &ccpars_meas.i_sim_tone_amp         }, 1 },
    { "B_SIM_TONE_AMP",        PAR_FLOAT,    1, 0, NULL,            { .f = &ccpars_meas.b_sim_tone_amp         }, 1 },
    { NULL }
}
#endif
;

#endif
// EOF
