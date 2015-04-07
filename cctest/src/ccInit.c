/*---------------------------------------------------------------------------------------------------------*\
  File:     ccInit.c                                                                    Copyright CERN 2015

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

  Purpose:  cctest simulation initialisation functions

  Authors:  Quentin.King@cern.ch

\*---------------------------------------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <math.h>
#include <string.h>

// Include cctest program header files

#include "ccCmds.h"
#include "ccTest.h"
#include "ccParse.h"
#include "ccRef.h"
#include "ccLog.h"
#include "ccRun.h"

/*---------------------------------------------------------------------------------------------------------*/
void ccInitPars(void)
/*---------------------------------------------------------------------------------------------------------*\
  This function is called after startup to allocate space to store the number of elements for each parameter.
  For CYC_SEL parameters, this is an array while for non-CYC_SEL parameters is it scalar. For CYC_SEL
  parameters it also copies the initialisation value from (0) to all the cycle selector slots.
\*---------------------------------------------------------------------------------------------------------*/
{
    struct CCpars *par;
    struct cccmds *cmd;

    for(cmd = cmds ; cmd->name != NULL ; cmd++)
    {
        par = cmd->pars;

        if(par != NULL)
        {
            while(par->name != NULL)
            {
                bool is_cyc_sel_par = par->cyc_sel_step != 0;

                // Allocate the space for the number of elements and initialise just the first number of elements
                // from default value

                par->num_elements    = calloc((is_cyc_sel_par ? CC_NUM_CYC_SELS : 1), sizeof(uint32_t));
                par->num_elements[0] = par->num_default_elements;

                // For CYC_SEL parameters, initialise value for every CYC_SEL from the default value

                if(is_cyc_sel_par)
                {
                    uint32_t cyc_sel;

                    for(cyc_sel = 1 ; cyc_sel <= CC_MAX_CYC_SEL ; cyc_sel++)
                    {
                        par->num_elements[cyc_sel] = par->num_default_elements;

                        memcpy(&par->value_p.c[cyc_sel * par->cyc_sel_step],
                               &par->value_p.c[0],
                               ccpars_sizeof_type[par->type] * par->num_default_elements);
                    }
                }

                par++;
            }
        }
    }
}
/*---------------------------------------------------------------------------------------------------------*/
uint32_t ccInitFunctions(void)
/*---------------------------------------------------------------------------------------------------------*\
  This function is called when the RUN command is executed to prepare for the new run.  It checks that
  parameters are valid and initialises the functions.
\*---------------------------------------------------------------------------------------------------------*/
{
    uint32_t         idx;
    uint32_t         cyc_sel;
    uint32_t         exit_status;
    struct fgfunc   *func;
    struct cccmds   *cmd;

    // Initialise ccrun structure

    memset(&ccrun, 0, sizeof(ccrun));

    ccrun.is_breg_enabled = false;
    ccrun.is_ireg_enabled = false;

    ccrun.num_cycles = global_pars[GLOBAL_CYCLE_SELECTOR].num_elements[0];

    // Reset is_enabled flags for all commands

    for(cmd = cmds ; cmd->name != NULL ; cmd++)
    {
        cmd->is_enabled = false;
    }

    // if GLOBAL REVERSE_TIME is ENALBED

    if(ccpars_global.reverse_time == REG_ENABLED)
    {
        // SIM_LOAD must be DISABLED

        if(ccpars_global.sim_load == REG_ENABLED)
        {
            ccParsPrintError("GLOBAL SIM_LOAD must be DISABLED when REVERSE_TIME is ENABLED");
            return(EXIT_FAILURE);
        }

        // Only one function can be specified

        if(ccrun.num_cycles > 1)
        {
            ccParsPrintError("only one function can be specified when REVERSE_TIME is ENABLED");
            return(EXIT_FAILURE);
        }

        // CSV output must be enabled as only CSV data will be written

        if(ccpars_global.csv_output != REG_ENABLED)
        {
            ccParsPrintError("GLOBAL CSV_OUTPUT must be ENABLED when REVERSE_TIME is ENABLED");
            return(EXIT_FAILURE);
        }

        // HTML output must be disabled as only CSV data will be written

        if(ccpars_global.html_output == REG_ENABLED)
        {
            ccParsPrintError("GLOBAL HTML_OUTPUT must be DISABLED when REVERSE_TIME is ENABLED");
            return(EXIT_FAILURE);
        }
    }

    // if GLOBAL FG_LIMITS is ENABLED then link to function generation limits

    if(ccpars_global.fg_limits == REG_ENABLED)
    {
        ccrun.fg_func_limits = &ccrun.fg_limits;
    }

    // If voltage perturbation is not required then set perturb_time to beyond end of simulation

    if(ccpars_load.perturb_time <= 0.0 || ccpars_load.perturb_volts == 0.0)
    {
        ccpars_load.perturb_volts = 0.0;
        ccpars_load.perturb_time  = 1.0E30;
    }

    // Initialise the reference functions

    exit_status = EXIT_SUCCESS;

    for(idx = 0 ; idx < ccrun.num_cycles ; idx++)
    {
        // Set cyc_sel and reg_rst_source taking GLOBAL TEST_CYC_SEL and GLOBAL TEST_REF_CYC_SEL into account

        cyc_sel = ccpars_global.cycle_selector[idx]; 

        ccrun.cycle[idx].reg_rst_source = ccpars_global.test_cyc_sel == 0 || ccpars_global.test_cyc_sel != cyc_sel ? 
                                          REG_OPERATIONAL_RST_PARS : REG_TEST_RST_PARS;

        if(ccrun.cycle[idx].reg_rst_source == REG_TEST_RST_PARS && ccpars_global.test_ref_cyc_sel > 0)
        {
            cyc_sel = ccpars_global.test_ref_cyc_sel;
        }

        ccrun.cycle[idx].cyc_sel = cyc_sel;

        // If function this cycle selector has not yet been initialised

        if(ccrun.is_used[cyc_sel] == false)
        {
            // Check that FUNCTION is not NONE

            if(ccpars_ref[cyc_sel].function == FG_NONE)
            {
                ccParsPrintError("REF FUNCTION(%u) must not be NONE", cyc_sel);
                return(EXIT_FAILURE);
            }

            // Check that reg_mode is compatible with PC ACTUATION

            if(ccpars_pc.actuation == REG_CURRENT_REF)
            {
                if(ccpars_ref[cyc_sel].reg_mode != REG_CURRENT)
                {
                    ccParsPrintError("REF REG_MODE(%u) must CURRENT when GLOBAL ACTUATION is CURRENT", cyc_sel);
                    return(EXIT_FAILURE);
                }
            }

            // Initialise pointer to function generation limits

            switch(ccpars_ref[cyc_sel].reg_mode)
            {
                case REG_NONE: break;
                case REG_FIELD:

                    ccrun.is_breg_enabled        = true;
                    ccrun.fg_limits.pos          = ccpars_limits.b_pos         [ccpars_load.select];
                    ccrun.fg_limits.min          = ccpars_limits.b_min         [ccpars_load.select];
                    ccrun.fg_limits.neg          = ccpars_limits.b_neg         [ccpars_load.select];
                    ccrun.fg_limits.rate         = ccpars_limits.b_rate        [ccpars_load.select];
                    ccrun.fg_limits.acceleration = ccpars_limits.b_acceleration[ccpars_load.select];
                    break;

                case REG_CURRENT:

                    ccrun.is_ireg_enabled        = true;
                    ccrun.fg_limits.pos          = ccpars_limits.i_pos         [ccpars_load.select];
                    ccrun.fg_limits.min          = ccpars_limits.i_min         [ccpars_load.select];
                    ccrun.fg_limits.neg          = ccpars_limits.i_neg         [ccpars_load.select];
                    ccrun.fg_limits.rate         = ccpars_limits.i_rate        [ccpars_load.select];
                    ccrun.fg_limits.acceleration = ccpars_limits.i_acceleration[ccpars_load.select];
                    break;

                case REG_VOLTAGE:

                    ccrun.fg_limits.pos          = ccpars_limits.v_pos         [ccpars_load.select];
                    ccrun.fg_limits.min          = 0.0;
                    ccrun.fg_limits.neg          = ccpars_limits.v_neg         [ccpars_load.select];
                    ccrun.fg_limits.rate         = ccpars_limits.v_rate;
                    ccrun.fg_limits.acceleration = ccpars_limits.v_acceleration;
                    break;
            }

            // Try to arm the function for this cycle selector

            func = &funcs[ccpars_ref[cyc_sel].function];

            if(func->init_func(&ccrun.fg_error[cyc_sel], cyc_sel) != FG_OK)
            {
                ccParsPrintError("failed to initialise %s(%u) : %s : error_idx=%u : error_data=%g,%g,%g,%g", 
                        ccParsEnumString(enum_function_type, ccpars_ref[cyc_sel].function),
                        cyc_sel,
                        ccParsEnumString(enum_fg_error, ccrun.fg_error[cyc_sel].fg_errno),
                        ccrun.fg_error[cyc_sel].index,
                        ccrun.fg_error[cyc_sel].data[0],ccrun.fg_error[cyc_sel].data[1],
                        ccrun.fg_error[cyc_sel].data[2],ccrun.fg_error[cyc_sel].data[3]);
                exit_status = EXIT_FAILURE;
            }

            // Mark command for this function as enabled to include parameters in FLOT colorbox pop-up

            cmds[func->cmd_idx].is_enabled = true;
            ccrun.is_used[cyc_sel] = true;
        }
    }

    // Check that no errors occurred while arming functions

    if(exit_status == EXIT_FAILURE)
    {
        ccParsPrintError("unable to arm one or more functions");
        return(EXIT_FAILURE);
    }

    // Check if REG_MODE is FIELD or CURRENT that SIM_LOAD is ENABLED

    if((ccrun.is_breg_enabled == true || ccrun.is_ireg_enabled == true) && ccpars_global.sim_load != REG_ENABLED)
    {
        ccParsPrintError("GLOBAL SIM_LOAD must be ENABLED if REG_MODE is FIELD or CURRENT");
        return(EXIT_FAILURE);
    }

    // Check that all selected functions were armed

    for(idx = 0 ; idx < ccrun.num_cycles ; idx++)
    {
        if(ccpars_ref[ccpars_global.cycle_selector[idx]].function == FG_NONE)
        {
            ccParsPrintError("cycle %u of %u (selector %u) is not armed", idx, ccrun.num_cycles, ccpars_global.cycle_selector[idx]);
            return(EXIT_FAILURE);
        }
    }

    // Enable the commands whose parameters should be included in debug output

    cmds[CMD_GLOBAL].is_enabled = true;
    cmds[CMD_LIMITS].is_enabled = (ccpars_global.fg_limits == REG_ENABLED);

    // Initialise converter structure

    free(reg_mgr.b.meas.fir_buf[0]);
    free(reg_mgr.i.meas.fir_buf[0]);

    memset(&reg_mgr, 0, sizeof(reg_mgr));

    regMgrInit(&reg_mgr,
               ccpars_global.iter_period_us,
               ccrun.is_breg_enabled,
               ccrun.is_ireg_enabled,
               false); // ccpars_pc.actuation == REG_FIRING_REF);

    return(EXIT_SUCCESS);
}
/*---------------------------------------------------------------------------------------------------------*/
uint32_t ccInitSimLoad(void)
/*---------------------------------------------------------------------------------------------------------*\
  This function is called when the RUN command is executed to prepare for the new run.  It checks that
  parameters are valid and initialises the simulation.
\*---------------------------------------------------------------------------------------------------------*/
{
    size_t      buf_len;
    uint32_t    exit_status = EXIT_SUCCESS;
    static struct REG_meas_signal invalid_meas = { 0.0, false };

    // Enabled of commands whose parameters should be included in debug output

    cmds[CMD_DEFAULT].is_enabled = true;
    cmds[CMD_LOAD   ].is_enabled = true;
    cmds[CMD_LIMITS ].is_enabled = true;
    cmds[CMD_MEAS   ].is_enabled = true;
    cmds[CMD_PC     ].is_enabled = true;
    cmds[CMD_REF    ].is_enabled = true;
    cmds[CMD_BREG   ].is_enabled = ccrun.is_breg_enabled;
    cmds[CMD_IREG   ].is_enabled = ccrun.is_ireg_enabled;

    // Set random_threshold for good measurements based on MEAS PROB_INVALID_SIGNAL

    if(ccpars_meas.invalid_probability > 0.0 && ccpars_meas.invalid_probability < 1.0)
    {
        ccrun.invalid_meas.random_threshold = (long)(RAND_MAX * ccpars_meas.invalid_probability);
    }
    
    // Prepare an invalid signal to allow recovery from invalid signals to be tested

    regMgrMeasInit(&reg_mgr, &invalid_meas, &invalid_meas, &invalid_meas);

    // Prepare measurement FIR buffers

    buf_len = ccpars_meas.b_fir_lengths[0] + ccpars_meas.b_fir_lengths[1] + ccpars_breg.period_iters[ccpars_load.select];

    regMeasFilterInitBuffer(&reg_mgr.b.meas, calloc(buf_len,sizeof(int32_t)), buf_len);

    // Initialise current measurement filter

    buf_len = ccpars_meas.i_fir_lengths[0] + ccpars_meas.i_fir_lengths[1] + ccpars_ireg.period_iters[ccpars_load.select];

    regMeasFilterInitBuffer(&reg_mgr.i.meas, calloc(buf_len,sizeof(int32_t)), buf_len);

    // Initialise libreg parameter pointers to cctest variables

    regMgrParInitPointer(&reg_mgr,   reg_err_rate                  ,&ccpars_global.reg_err_rate);

    regMgrParInitPointer(&reg_mgr,   breg_period_iters             , ccpars_breg.period_iters);
    regMgrParInitPointer(&reg_mgr,   breg_pure_delay_periods       , ccpars_breg.pure_delay_periods);
    regMgrParInitPointer(&reg_mgr,   breg_track_delay_periods      , ccpars_breg.track_delay_periods);
    regMgrParInitPointer(&reg_mgr,   breg_auxpole1_hz              , ccpars_breg.auxpole1_hz);
    regMgrParInitPointer(&reg_mgr,   breg_auxpoles2_hz             , ccpars_breg.auxpoles2_hz);
    regMgrParInitPointer(&reg_mgr,   breg_auxpoles2_z              , ccpars_breg.auxpoles2_z);
    regMgrParInitPointer(&reg_mgr,   breg_auxpole4_hz              , ccpars_breg.auxpole4_hz);
    regMgrParInitPointer(&reg_mgr,   breg_auxpole5_hz              , ccpars_breg.auxpole5_hz);
    regMgrParInitPointer(&reg_mgr,   breg_r                        , ccpars_breg.rst.r);
    regMgrParInitPointer(&reg_mgr,   breg_s                        , ccpars_breg.rst.s);
    regMgrParInitPointer(&reg_mgr,   breg_t                        , ccpars_breg.rst.t);

    regMgrParInitPointer(&reg_mgr,   ireg_period_iters             , ccpars_ireg.period_iters);
    regMgrParInitPointer(&reg_mgr,   ireg_pure_delay_periods       , ccpars_ireg.pure_delay_periods);
    regMgrParInitPointer(&reg_mgr,   ireg_track_delay_periods      , ccpars_ireg.track_delay_periods);
    regMgrParInitPointer(&reg_mgr,   ireg_auxpole1_hz              , ccpars_ireg.auxpole1_hz);
    regMgrParInitPointer(&reg_mgr,   ireg_auxpoles2_hz             , ccpars_ireg.auxpoles2_hz);
    regMgrParInitPointer(&reg_mgr,   ireg_auxpoles2_z              , ccpars_ireg.auxpoles2_z);
    regMgrParInitPointer(&reg_mgr,   ireg_auxpole4_hz              , ccpars_ireg.auxpole4_hz);
    regMgrParInitPointer(&reg_mgr,   ireg_auxpole5_hz              , ccpars_ireg.auxpole5_hz);
    regMgrParInitPointer(&reg_mgr,   ireg_r                        , ccpars_ireg.rst.r);
    regMgrParInitPointer(&reg_mgr,   ireg_s                        , ccpars_ireg.rst.s);
    regMgrParInitPointer(&reg_mgr,   ireg_t                        , ccpars_ireg.rst.t);

    regMgrParInitPointer(&reg_mgr,   limits_b_pos                  , ccpars_limits.b_pos);
    regMgrParInitPointer(&reg_mgr,   limits_b_min                  , ccpars_limits.b_min);
    regMgrParInitPointer(&reg_mgr,   limits_b_neg                  , ccpars_limits.b_neg);
    regMgrParInitPointer(&reg_mgr,   limits_b_rate                 , ccpars_limits.b_rate);
    regMgrParInitPointer(&reg_mgr,   limits_b_acceleration         , ccpars_limits.b_acceleration);
    regMgrParInitPointer(&reg_mgr,   limits_b_closeloop            , ccpars_limits.b_closeloop);
    regMgrParInitPointer(&reg_mgr,   limits_b_low                  , ccpars_limits.b_low);
    regMgrParInitPointer(&reg_mgr,   limits_b_zero                 , ccpars_limits.b_zero);
    regMgrParInitPointer(&reg_mgr,   limits_b_err_warning          , ccpars_limits.b_err_warning);
    regMgrParInitPointer(&reg_mgr,   limits_b_err_fault            , ccpars_limits.b_err_fault);

    regMgrParInitPointer(&reg_mgr,   limits_i_pos                  , ccpars_limits.i_pos);
    regMgrParInitPointer(&reg_mgr,   limits_i_min                  , ccpars_limits.i_min);
    regMgrParInitPointer(&reg_mgr,   limits_i_neg                  , ccpars_limits.i_neg);
    regMgrParInitPointer(&reg_mgr,   limits_i_rate                 , ccpars_limits.i_rate);
    regMgrParInitPointer(&reg_mgr,   limits_i_acceleration         , ccpars_limits.i_acceleration);
    regMgrParInitPointer(&reg_mgr,   limits_i_closeloop            , ccpars_limits.i_closeloop);
    regMgrParInitPointer(&reg_mgr,   limits_i_low                  , ccpars_limits.i_low);
    regMgrParInitPointer(&reg_mgr,   limits_i_zero                 , ccpars_limits.i_zero);
    regMgrParInitPointer(&reg_mgr,   limits_i_err_warning          , ccpars_limits.i_err_warning);
    regMgrParInitPointer(&reg_mgr,   limits_i_err_fault            , ccpars_limits.i_err_fault);

    regMgrParInitPointer(&reg_mgr,   limits_i_rms_tc               ,&ccpars_limits.i_rms_tc);
    regMgrParInitPointer(&reg_mgr,   limits_i_rms_warning          ,&ccpars_limits.i_rms_warning);
    regMgrParInitPointer(&reg_mgr,   limits_i_rms_fault            ,&ccpars_limits.i_rms_fault);
    regMgrParInitPointer(&reg_mgr,   limits_i_rms_load_tc          , ccpars_limits.i_rms_load_tc);
    regMgrParInitPointer(&reg_mgr,   limits_i_rms_load_warning     , ccpars_limits.i_rms_load_warning);
    regMgrParInitPointer(&reg_mgr,   limits_i_rms_load_fault       , ccpars_limits.i_rms_load_fault);

    regMgrParInitPointer(&reg_mgr,   limits_i_quadrants41          , ccpars_limits.i_quadrants41);
    regMgrParInitPointer(&reg_mgr,   limits_v_pos                  , ccpars_limits.v_pos);
    regMgrParInitPointer(&reg_mgr,   limits_v_neg                  , ccpars_limits.v_neg);
    regMgrParInitPointer(&reg_mgr,   limits_v_rate                 ,&ccpars_limits.v_rate);
    regMgrParInitPointer(&reg_mgr,   limits_v_acceleration         ,&ccpars_limits.v_acceleration);
    regMgrParInitPointer(&reg_mgr,   limits_v_err_warning          ,&ccpars_limits.v_err_warning);
    regMgrParInitPointer(&reg_mgr,   limits_v_err_fault            ,&ccpars_limits.v_err_fault);
    regMgrParInitPointer(&reg_mgr,   limits_v_quadrants41          , ccpars_limits.v_quadrants41);
    regMgrParInitPointer(&reg_mgr,   limits_invert                 ,&ccpars_limits.invert);

    regMgrParInitPointer(&reg_mgr,   load_ohms_ser                 , ccpars_load.ohms_ser);
    regMgrParInitPointer(&reg_mgr,   load_ohms_par                 , ccpars_load.ohms_par);
    regMgrParInitPointer(&reg_mgr,   load_ohms_mag                 , ccpars_load.ohms_mag);
    regMgrParInitPointer(&reg_mgr,   load_henrys                   , ccpars_load.henrys);
    regMgrParInitPointer(&reg_mgr,   load_henrys_sat               , ccpars_load.henrys_sat);
    regMgrParInitPointer(&reg_mgr,   load_i_sat_start              , ccpars_load.i_sat_start);
    regMgrParInitPointer(&reg_mgr,   load_i_sat_end                , ccpars_load.i_sat_end);
    regMgrParInitPointer(&reg_mgr,   load_gauss_per_amp            , ccpars_load.gauss_per_amp);
    regMgrParInitPointer(&reg_mgr,   load_select                   ,&ccpars_load.select);
    regMgrParInitPointer(&reg_mgr,   load_test_select              ,&ccpars_load.test_select);
    regMgrParInitPointer(&reg_mgr,   load_sim_tc_error             ,&ccpars_load.sim_tc_error);

    regMgrParInitPointer(&reg_mgr,   meas_b_reg_select             ,&ccpars_meas.b_reg_select);
    regMgrParInitPointer(&reg_mgr,   meas_i_reg_select             ,&ccpars_meas.i_reg_select);
    regMgrParInitPointer(&reg_mgr,   meas_b_delay_iters            ,&ccpars_meas.b_delay_iters);
    regMgrParInitPointer(&reg_mgr,   meas_i_delay_iters            ,&ccpars_meas.i_delay_iters);
    regMgrParInitPointer(&reg_mgr,   meas_v_delay_iters            ,&ccpars_meas.v_delay_iters);
    regMgrParInitPointer(&reg_mgr,   meas_b_fir_lengths            , ccpars_meas.b_fir_lengths);
    regMgrParInitPointer(&reg_mgr,   meas_i_fir_lengths            , ccpars_meas.i_fir_lengths);
    regMgrParInitPointer(&reg_mgr,   meas_b_sim_noise_pp           ,&ccpars_meas.b_sim_noise_pp);
    regMgrParInitPointer(&reg_mgr,   meas_i_sim_noise_pp           ,&ccpars_meas.i_sim_noise_pp);
    regMgrParInitPointer(&reg_mgr,   meas_v_sim_noise_pp           ,&ccpars_meas.v_sim_noise_pp);
    regMgrParInitPointer(&reg_mgr,   meas_b_sim_quantization       ,&ccpars_meas.b_sim_quantization);
    regMgrParInitPointer(&reg_mgr,   meas_i_sim_quantization       ,&ccpars_meas.i_sim_quantization);
    regMgrParInitPointer(&reg_mgr,   meas_v_sim_quantization       ,&ccpars_meas.v_sim_quantization);

    regMgrParInitPointer(&reg_mgr,   meas_sim_tone_period_iters    ,&ccpars_meas.sim_tone_period_iters);
    regMgrParInitPointer(&reg_mgr,   meas_b_sim_tone_pp            ,&ccpars_meas.b_sim_tone_pp);
    regMgrParInitPointer(&reg_mgr,   meas_i_sim_tone_pp            ,&ccpars_meas.i_sim_tone_pp);

    regMgrParInitPointer(&reg_mgr,   pc_actuation                  ,&ccpars_pc.actuation);
    regMgrParInitPointer(&reg_mgr,   pc_act_delay_iters            ,&ccpars_pc.act_delay_iters);
    regMgrParInitPointer(&reg_mgr,   pc_sim_bandwidth              ,&ccpars_pc.sim_bandwidth);
    regMgrParInitPointer(&reg_mgr,   pc_sim_z                      ,&ccpars_pc.sim_z);
    regMgrParInitPointer(&reg_mgr,   pc_sim_tau_zero               ,&ccpars_pc.sim_tau_zero);
    regMgrParInitPointer(&reg_mgr,   pc_sim_num                    , ccpars_pc.sim_pc_pars.num);
    regMgrParInitPointer(&reg_mgr,   pc_sim_den                    , ccpars_pc.sim_pc_pars.den);
    regMgrParInitPointer(&reg_mgr,   pc_sim_quantization           ,&ccpars_pc.sim_quantization);
    regMgrParInitPointer(&reg_mgr,   pc_sim_noise_pp               ,&ccpars_pc.sim_noise_pp);
    regMgrParInitPointer(&reg_mgr,   pc_sim_tone_period_iters      ,&ccpars_pc.sim_tone_period_iters);
    regMgrParInitPointer(&reg_mgr,   pc_sim_tone_pp                ,&ccpars_pc.sim_tone_pp);

    // Initialise simulation

    regMgrSimInit(&reg_mgr, ccpars_ref[ccpars_global.cycle_selector[0]].reg_mode,
                          ccrun.fg_pars[ccpars_global.cycle_selector[0]].meta.range.initial_ref);

    // Check simulated voltage source gain

    if(fabs(reg_mgr.sim_pc_pars.gain - 1.0) > 0.05)
    {
        ccParsPrintError("voltage source model gain (%.3f) has an error of more than 5%%", reg_mgr.sim_pc_pars.gain);

        exit_status = EXIT_FAILURE;
    }

    // Check initialisation of field regulation

    if(ccrun.is_breg_enabled == true)
    {
        if(reg_mgr.iter_period > (3.0 * reg_mgr.load_pars.tc))
        {
            ccParsPrintError("REG_MODE FIELD is not permitted for a resistive circuit "
                             "(circuit time constant is less than 1/3 x iteration period)");

            exit_status = EXIT_FAILURE;
        }

        if(reg_mgr.b.last_op_rst_pars.status == REG_FAULT)
        {
            ccParsPrintError("failed to initialise operational FIELD RST regulator: %s",
                            ccParsEnumString(enum_reg_jurys_result, reg_mgr.b.last_op_rst_pars.jurys_result));

            exit_status = EXIT_FAILURE;
        }

        if(ccpars_global.test_cyc_sel > 0 && reg_mgr.b.last_test_rst_pars.status == REG_FAULT)
        {
            ccParsPrintError("failed to initialise test FIELD RST regulator: %s",
                            ccParsEnumString(enum_reg_jurys_result, reg_mgr.b.last_test_rst_pars.jurys_result));

            exit_status = EXIT_FAILURE;
        }

        if(reg_mgr.b.last_op_rst_pars.status == REG_WARNING)
        {
            ccParsPrintError("FIELD RST regulator warning: Modulus Margin (%.2f) is less than %.1f - try reducing AUXPOLE frequencies",
                             reg_mgr.b.last_op_rst_pars.modulus_margin, REG_MM_WARNING_THRESHOLD);
        }
    }

    // Check initialisation of current regulation

    if(ccrun.is_ireg_enabled == true)
    {
        if(reg_mgr.i.last_op_rst_pars.status == REG_FAULT)
        {
            ccParsPrintError("failed to initialise operational CURRENT RST regulator: %s",
                            ccParsEnumString(enum_reg_jurys_result, reg_mgr.i.last_op_rst_pars.jurys_result));

            exit_status = EXIT_FAILURE;
        }

        if(ccpars_global.test_cyc_sel > 0 && reg_mgr.i.last_test_rst_pars.status == REG_FAULT)
        {
            ccParsPrintError("failed to initialise test CURRENT RST regulator: %s",
                            ccParsEnumString(enum_reg_jurys_result, reg_mgr.i.last_test_rst_pars.jurys_result));

            exit_status = EXIT_FAILURE;
        }

        if(reg_mgr.i.last_op_rst_pars.status == REG_WARNING)
        {
            ccParsPrintError("CURRENT RST regulator warning: Modulus Margin (%.2f) is less than %.1f - try reducing AUXPOLE frequencies",
                             reg_mgr.i.last_op_rst_pars.modulus_margin, REG_MM_WARNING_THRESHOLD);
        }
    }

    return(exit_status);
}
// EOF
