/*!
 * @file  ccrt/scr/ccInit.c
 *
 * @brief ccrt initialisation functions
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

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <math.h>
#include <string.h>

// Include cctest program header files

#include "ccCmds.h"
#include "ccRt.h"
#include "ccParse.h"
#include "ccRef.h"
#include "ccLog.h"
#include "ccRun.h"

//    ccrun.num_cycles = global_pars[GLOBAL_CYCLE_SELECTOR].num_elements[0];


// Set random_threshold for good measurements based on MEAS PROB_INVALID_SIGNAL

//    if(ccpars_meas.invalid_probability > 0.0 && ccpars_meas.invalid_probability < 1.0)
//    {
//        ccrun.invalid_meas.random_threshold = (long)(RAND_MAX * ccpars_meas.invalid_probability);
//    }

// Add invalid signal fault to libreg


void ccInitPars(void)
{
    struct ccpars  *par;
    struct cccmds  *cmd;
    static struct   reg_meas_signal invalid_meas = { 0.0, false };

    // Allocate space for the number of elements arrays for all the parameters

    for(cmd = cmds ; cmd->name != NULL ; cmd++)
    {
        par = cmd->pars;

        if(par != NULL)
        {
            while(par->name != NULL)
            {
                bool is_cyc_sel_par = par->cyc_sel_step != 0;

                // Accumulate the flags from all parameters to set the summary flags for the command

                cmd->flags |= par->flags;

                // Allocate the space for the number of elements and initialise just the first number of elements
                // from default value

                par->num_elements    = calloc((is_cyc_sel_par ? CC_NUM_CYC_SELS : 1), sizeof(uint32_t));
                par->num_elements[0] = par->num_default_elements;

                // For cycle selector parameters, initialise value for every cycle selector from the default value

                if(is_cyc_sel_par)
                {
                    uint32_t cyc_sel;

                    for(cyc_sel = 1 ; cyc_sel <= CC_MAX_CYC_SEL ; cyc_sel++)
                    {
                        par->num_elements[cyc_sel] = par->num_default_elements;

                        // Initialise value for read/write properties only

                        if((par->flags & PARS_RW) != 0)
                        {
                            memcpy(&par->value_p.c[cyc_sel * par->cyc_sel_step],
                                   &par->value_p.c[0],
                                   ccpars_sizeof_type[par->type] * par->num_default_elements);
                        }
                    }
                }

                par++;
            }
        }
    }

    // Initialise Regulation library's conv structure

    regConvInit(&conv, CC_ITER_PERIOD_US, true, true);      // Support field and current regulation (true, true)

    // Prepare an invalid signal to allow recovery from invalid signals to be tested

    regConvMeasInit(&conv, &invalid_meas, &invalid_meas, &invalid_meas);

    // Prepare measurement FIR buffers

    regMeasFilterInitBuffer(&conv.b.meas, calloc(CC_FILTER_BUF_LEN, sizeof(int32_t)), CC_FILTER_BUF_LEN);

    // Initialise current measurement filter

    regMeasFilterInitBuffer(&conv.i.meas, calloc(CC_FILTER_BUF_LEN, sizeof(int32_t)), CC_FILTER_BUF_LEN);

    // Initialise libreg parameter pointers to cctest variables

    regConvParInitPointer(&conv,    reg_err_rate                  ,&ccpars_global.reg_err_rate);

    regConvParInitPointer(&conv,    breg_period_iters             ,&ccpars_breg.period_iters);
    regConvParInitPointer(&conv,    breg_pure_delay_periods       ,&ccpars_breg.pure_delay_periods);
    regConvParInitPointer(&conv,    breg_track_delay_periods      ,&ccpars_breg.track_delay_periods);
    regConvParInitPointer(&conv,    breg_auxpole1_hz              ,&ccpars_breg.auxpole1_hz);
    regConvParInitPointer(&conv,    breg_auxpoles2_hz             ,&ccpars_breg.auxpoles2_hz);
    regConvParInitPointer(&conv,    breg_auxpoles2_z              ,&ccpars_breg.auxpoles2_z);
    regConvParInitPointer(&conv,    breg_auxpole4_hz              ,&ccpars_breg.auxpole4_hz);
    regConvParInitPointer(&conv,    breg_auxpole5_hz              ,&ccpars_breg.auxpole5_hz);
    regConvParInitPointer(&conv,    breg_r                        ,&ccpars_breg.rst.r);
    regConvParInitPointer(&conv,    breg_s                        ,&ccpars_breg.rst.s);
    regConvParInitPointer(&conv,    breg_t                        ,&ccpars_breg.rst.t);

    regConvParInitPointer(&conv,    ireg_period_iters             ,&ccpars_ireg.period_iters);
    regConvParInitPointer(&conv,    ireg_pure_delay_periods       ,&ccpars_ireg.pure_delay_periods);
    regConvParInitPointer(&conv,    ireg_track_delay_periods      ,&ccpars_ireg.track_delay_periods);
    regConvParInitPointer(&conv,    ireg_auxpole1_hz              ,&ccpars_ireg.auxpole1_hz);
    regConvParInitPointer(&conv,    ireg_auxpoles2_hz             ,&ccpars_ireg.auxpoles2_hz);
    regConvParInitPointer(&conv,    ireg_auxpoles2_z              ,&ccpars_ireg.auxpoles2_z);
    regConvParInitPointer(&conv,    ireg_auxpole4_hz              ,&ccpars_ireg.auxpole4_hz);
    regConvParInitPointer(&conv,    ireg_auxpole5_hz              ,&ccpars_ireg.auxpole5_hz);
    regConvParInitPointer(&conv,    ireg_r                        ,&ccpars_ireg.rst.r);
    regConvParInitPointer(&conv,    ireg_s                        ,&ccpars_ireg.rst.s);
    regConvParInitPointer(&conv,    ireg_t                        ,&ccpars_ireg.rst.t);

    regConvParInitPointer(&conv,    limits_b_pos                  ,&ccpars_limits.b_pos);
    regConvParInitPointer(&conv,    limits_b_min                  ,&ccpars_limits.b_min);
    regConvParInitPointer(&conv,    limits_b_neg                  ,&ccpars_limits.b_neg);
    regConvParInitPointer(&conv,    limits_b_rate                 ,&ccpars_limits.b_rate);
    regConvParInitPointer(&conv,    limits_b_acceleration         ,&ccpars_limits.b_acceleration);
    regConvParInitPointer(&conv,    limits_b_closeloop            ,&ccpars_limits.b_closeloop);
    regConvParInitPointer(&conv,    limits_b_low                  ,&ccpars_limits.b_low);
    regConvParInitPointer(&conv,    limits_b_zero                 ,&ccpars_limits.b_zero);
    regConvParInitPointer(&conv,    limits_b_err_warning          ,&ccpars_limits.b_err_warning);
    regConvParInitPointer(&conv,    limits_b_err_fault            ,&ccpars_limits.b_err_fault);

    regConvParInitPointer(&conv,    limits_i_pos                  ,&ccpars_limits.i_pos);
    regConvParInitPointer(&conv,    limits_i_min                  ,&ccpars_limits.i_min);
    regConvParInitPointer(&conv,    limits_i_neg                  ,&ccpars_limits.i_neg);
    regConvParInitPointer(&conv,    limits_i_rate                 ,&ccpars_limits.i_rate);
    regConvParInitPointer(&conv,    limits_i_acceleration         ,&ccpars_limits.i_acceleration);
    regConvParInitPointer(&conv,    limits_i_closeloop            ,&ccpars_limits.i_closeloop);
    regConvParInitPointer(&conv,    limits_i_low                  ,&ccpars_limits.i_low);
    regConvParInitPointer(&conv,    limits_i_zero                 ,&ccpars_limits.i_zero);
    regConvParInitPointer(&conv,    limits_i_err_warning          ,&ccpars_limits.i_err_warning);
    regConvParInitPointer(&conv,    limits_i_err_fault            ,&ccpars_limits.i_err_fault);

    regConvParInitPointer(&conv,    limits_i_rms_tc               ,&ccpars_limits.i_rms_tc);
    regConvParInitPointer(&conv,    limits_i_rms_warning          ,&ccpars_limits.i_rms_warning);
    regConvParInitPointer(&conv,    limits_i_rms_fault            ,&ccpars_limits.i_rms_fault);
    regConvParInitPointer(&conv,    limits_i_rms_load_tc          ,&ccpars_limits.i_rms_load_tc);
    regConvParInitPointer(&conv,    limits_i_rms_load_warning     ,&ccpars_limits.i_rms_load_warning);
    regConvParInitPointer(&conv,    limits_i_rms_load_fault       ,&ccpars_limits.i_rms_load_fault);

    regConvParInitPointer(&conv,    limits_i_quadrants41          ,&ccpars_limits.i_quadrants41);
    regConvParInitPointer(&conv,    limits_v_pos                  ,&ccpars_limits.v_pos);
    regConvParInitPointer(&conv,    limits_v_neg                  ,&ccpars_limits.v_neg);
    regConvParInitPointer(&conv,    limits_v_rate                 ,&ccpars_limits.v_rate);
    regConvParInitPointer(&conv,    limits_v_acceleration         ,&ccpars_limits.v_acceleration);
    regConvParInitPointer(&conv,    limits_v_err_warning          ,&ccpars_limits.v_err_warning);
    regConvParInitPointer(&conv,    limits_v_err_fault            ,&ccpars_limits.v_err_fault);
    regConvParInitPointer(&conv,    limits_v_quadrants41          ,&ccpars_limits.v_quadrants41);
    regConvParInitPointer(&conv,    limits_invert                 ,&ccrun.polswitch.limits_invert);

    regConvParInitPointer(&conv,    load_ohms_ser                 ,&ccpars_load.ohms_ser);
    regConvParInitPointer(&conv,    load_ohms_par                 ,&ccpars_load.ohms_par);
    regConvParInitPointer(&conv,    load_ohms_mag                 ,&ccpars_load.ohms_mag);
    regConvParInitPointer(&conv,    load_henrys                   ,&ccpars_load.henrys);
    regConvParInitPointer(&conv,    load_henrys_sat               ,&ccpars_load.henrys_sat);
    regConvParInitPointer(&conv,    load_i_sat_start              ,&ccpars_load.i_sat_start);
    regConvParInitPointer(&conv,    load_i_sat_end                ,&ccpars_load.i_sat_end);
    regConvParInitPointer(&conv,    load_gauss_per_amp            ,&ccpars_load.gauss_per_amp);
    regConvParInitPointer(&conv,    load_select                   ,&ccpars_load.select);
    regConvParInitPointer(&conv,    load_test_select              ,&ccpars_load.test_select);
    regConvParInitPointer(&conv,    load_sim_tc_error             ,&ccpars_load.sim_tc_error);

    regConvParInitPointer(&conv,    meas_b_reg_select             ,&ccpars_meas.b_reg_select);
    regConvParInitPointer(&conv,    meas_i_reg_select             ,&ccpars_meas.i_reg_select);
    regConvParInitPointer(&conv,    meas_b_delay_iters            ,&ccpars_meas.b_delay_iters);
    regConvParInitPointer(&conv,    meas_i_delay_iters            ,&ccpars_meas.i_delay_iters);
    regConvParInitPointer(&conv,    meas_v_delay_iters            ,&ccpars_meas.v_delay_iters);
    regConvParInitPointer(&conv,    meas_b_fir_lengths            ,&ccpars_meas.b_fir_lengths);
    regConvParInitPointer(&conv,    meas_i_fir_lengths            ,&ccpars_meas.i_fir_lengths);
    regConvParInitPointer(&conv,    meas_b_sim_noise_pp           ,&ccpars_meas.b_sim_noise_pp);
    regConvParInitPointer(&conv,    meas_i_sim_noise_pp           ,&ccpars_meas.i_sim_noise_pp);
    regConvParInitPointer(&conv,    meas_v_sim_noise_pp           ,&ccpars_meas.v_sim_noise_pp);
    regConvParInitPointer(&conv,    meas_tone_half_period_iters   ,&ccpars_meas.tone_half_period_iters);
    regConvParInitPointer(&conv,    meas_b_sim_tone_amp           ,&ccpars_meas.b_sim_tone_amp);
    regConvParInitPointer(&conv,    meas_i_sim_tone_amp           ,&ccpars_meas.i_sim_tone_amp);

    regConvParInitPointer(&conv,    pc_actuation                  ,&ccpars_pc.actuation);
    regConvParInitPointer(&conv,    pc_act_delay_iters            ,&ccpars_pc.act_delay_iters);
    regConvParInitPointer(&conv,    pc_bandwidth                  ,&ccpars_pc.bandwidth);
    regConvParInitPointer(&conv,    pc_z                          ,&ccpars_pc.z);
    regConvParInitPointer(&conv,    pc_tau_zero                   ,&ccpars_pc.tau_zero);
    regConvParInitPointer(&conv,    pc_sim_num                    ,&ccpars_pc.sim_pc_pars.num);
    regConvParInitPointer(&conv,    pc_sim_den                    ,&ccpars_pc.sim_pc_pars.den);
}



uint32_t ccInitFunction(uint32_t cyc_sel)
{
    // Check that reg_mode is compatible with PC ACTUATION

    if(ccpars_pc.actuation == REG_CURRENT_REF)
    {
        if(ccpars_ref[cyc_sel].reg_mode != REG_CURRENT)
        {
            ccParsPrintError("REF REG_MODE(%u) must be CURRENT when GLOBAL ACTUATION is CURRENT", cyc_sel);
            return(EXIT_FAILURE);
        }
    }

    // Initialise pointer to function generation limits

    switch(ccpars_ref[cyc_sel].reg_mode)
    {
        case REG_NONE: break;
        case REG_FIELD:

            ccrun.fgen_limits.pos          = ccpars_limits.b_pos         [ccpars_load.select];
            ccrun.fgen_limits.min          = ccpars_limits.b_min         [ccpars_load.select];
            ccrun.fgen_limits.neg          = ccpars_limits.b_neg         [ccpars_load.select];
            ccrun.fgen_limits.rate         = ccpars_limits.b_rate        [ccpars_load.select];
            ccrun.fgen_limits.acceleration = ccpars_limits.b_acceleration[ccpars_load.select];
            break;

        case REG_CURRENT:

            ccrun.fgen_limits.pos          = ccpars_limits.i_pos         [ccpars_load.select];
            ccrun.fgen_limits.min          = ccpars_limits.i_min         [ccpars_load.select];
            ccrun.fgen_limits.neg          = ccpars_limits.i_neg         [ccpars_load.select];
            ccrun.fgen_limits.rate         = ccpars_limits.i_rate        [ccpars_load.select];
            ccrun.fgen_limits.acceleration = ccpars_limits.i_acceleration[ccpars_load.select];
            break;

        case REG_VOLTAGE:

            ccrun.fgen_limits.pos          = ccpars_limits.v_pos         [ccpars_load.select];
            ccrun.fgen_limits.min          = 0.0;
            ccrun.fgen_limits.neg          = ccpars_limits.v_neg         [ccpars_load.select];
            ccrun.fgen_limits.rate         = ccpars_limits.v_rate;
            ccrun.fgen_limits.acceleration = ccpars_limits.v_acceleration;
            break;
    }

    // Try to arm the function for this cycle selector

    if(funcs[ccpars_ref[cyc_sel].function].init_func(&ccpars_ref[cyc_sel].fg_meta, cyc_sel) != FG_OK)
    {
        ccParsPrintError("failed to initialise %s(%u) : %s",
                ccParsEnumString(enum_function_type, ccpars_ref[cyc_sel].function),
                cyc_sel,
                ccParsEnumString(enum_fg_error, ccpars_ref[cyc_sel].fg_meta.fg_error));

        return(EXIT_FAILURE);
    }

    ccpars_ref[cyc_sel].armed_function = ccpars_ref[cyc_sel].function;
    ccpars_ref[cyc_sel].armed_reg_mode = ccpars_ref[cyc_sel].reg_mode;

    return(EXIT_SUCCESS);
}

// EOF
