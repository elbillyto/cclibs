/*!
 * @file  regMgr.c
 * @brief Converter Control Regulation library high-level management functions.
 *
 * <h2>Copyright</h2>
 *
 * Copyright CERN 2015. This project is released under the GNU Lesser General
 * Public License version 3.
 * 
 * <h2>License</h2>
 *
 * This file is part of libreg.
 *
 * libreg is free software: you can redistribute it and/or modify it under the
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

// Include header files

#include <stdio.h>
#include <string.h>
#include <math.h>
#include "libreg.h"
#include "libreg_pars_init.h"

// Static function declarations

static void regMgrModeSetNoneOrVoltageRT(struct REG_mgr *reg_mgr, enum REG_mode reg_mode);
static void regMgrSignalPrepareRT(struct REG_mgr *reg_mgr, enum REG_mode reg_mode, uint32_t unix_time, uint32_t us_time);



// Background functions - do not call these from the real-time thread or interrupt

void regMgrInit(struct REG_mgr           *reg_mgr,
                uint32_t                  iter_period_us,
                enum REG_enabled_disabled field_regulation,
                enum REG_enabled_disabled current_regulation,
                enum REG_enabled_disabled voltage_regulation)
{
    reg_mgr->iter_period_us = iter_period_us;
    reg_mgr->iter_period    = iter_period_us * 1.0E-6;

    reg_mgr->b.regulation   = field_regulation;
    reg_mgr->i.regulation   = current_regulation;
    reg_mgr->v.regulation   = voltage_regulation;

    reg_mgr->reg_mode       = REG_NONE;
    reg_mgr->reg_rst_source = REG_OPERATIONAL_RST_PARS;
    reg_mgr->reg_signal     = &reg_mgr->i;

    // Prepare regulation parameter next/active pointers

    reg_mgr->b.op_rst_pars.next            = &reg_mgr->b.op_rst_pars.pars[0];
    reg_mgr->b.op_rst_pars.active          = &reg_mgr->b.op_rst_pars.pars[1];
    reg_mgr->b.op_rst_pars.is_next_ready   = false;

    reg_mgr->i.op_rst_pars.next            = &reg_mgr->i.op_rst_pars.pars[0];
    reg_mgr->i.op_rst_pars.active          = &reg_mgr->i.op_rst_pars.pars[1];
    reg_mgr->i.op_rst_pars.is_next_ready   = false;

    reg_mgr->b.test_rst_pars.next          = &reg_mgr->b.test_rst_pars.pars[0];
    reg_mgr->b.test_rst_pars.active        = &reg_mgr->b.test_rst_pars.pars[1];
    reg_mgr->b.test_rst_pars.is_next_ready = false;

    reg_mgr->i.test_rst_pars.next          = &reg_mgr->i.test_rst_pars.pars[0];
    reg_mgr->i.test_rst_pars.active        = &reg_mgr->i.test_rst_pars.pars[1];
    reg_mgr->i.test_rst_pars.is_next_ready = false;

    reg_mgr->b.rst_pars = reg_mgr->b.op_rst_pars.active;
    reg_mgr->i.rst_pars = reg_mgr->i.op_rst_pars.active;

    regMgrModeSetNoneOrVoltageRT(reg_mgr, REG_NONE);

    // Initialise libreg parameter structures reg_mgr

    regMgrParsInit(reg_mgr);
}



static void regMgrRstInit( struct REG_mgr     *reg_mgr,
                           enum REG_mode       reg_mode,
                           enum REG_rst_source reg_rst_source,
                           uint32_t            reg_period_iters,
                           REG_float           auxpole1_hz,
                           REG_float           auxpoles2_hz,
                           REG_float           auxpoles2_z,
                           REG_float           auxpole4_hz,
                           REG_float           auxpole5_hz,
                           REG_float           pure_delay_periods,
                           REG_float           track_delay_periods,
                           REG_float           manual_r[REG_NUM_RST_COEFFS],
                           REG_float           manual_s[REG_NUM_RST_COEFFS],
                           REG_float           manual_t[REG_NUM_RST_COEFFS])
{
    struct REG_mgr_rst_pars *mgr_rst_pars;
    struct REG_mgr_signal   *reg_signal;
    struct REG_load_pars    *load_pars;
    struct REG_rst_pars     *rst_pars;
    struct REG_rst_pars     *reg_signal_last_rst_pars;

    // Set pointer to the reg_mgr_rst_pars structure for FIELD/CURRENT regulation with OPERATIONAL/TEST parameters

    reg_signal = (reg_mode == REG_FIELD ? &reg_mgr->b : &reg_mgr->i);

    if(reg_rst_source == REG_OPERATIONAL_RST_PARS)
    {
        mgr_rst_pars             = &reg_signal->op_rst_pars;
        reg_signal_last_rst_pars = &reg_signal->last_op_rst_pars;
        load_pars                = &reg_mgr->load_pars;
    }
    else // REG_TEST_PARS
    {
        mgr_rst_pars             = &reg_signal->test_rst_pars;
        reg_signal_last_rst_pars = &reg_signal->last_test_rst_pars;
        load_pars                = &reg_mgr->load_pars_test;
    }

    // Set pointer to the RST pars structure that will be initialised - this will be used below so that the newly
    // initialised structure can be avaialble for debugging through *reg_signal_last_rst_pars

    rst_pars = mgr_rst_pars->next;

    // Test RST parameters are forced to use the same period as the operational parameters, so there is only
    // one period for a signal (current or field).

    reg_signal->reg_period_iters = reg_period_iters;
    reg_signal->reg_period       = reg_mgr->iter_period * (REG_float)reg_period_iters;
    reg_signal->inv_reg_period   = 1.0 / reg_signal->reg_period;

    // When actuation is CURRENT_REF then don't try to initialise the RST regulation,
    // just prepare the periods so that the delayed_reg calculation works

    if(regMgrVarP(reg_mgr, PC_ACTUATION) == REG_CURRENT_REF)
    {
        rst_pars->status               = REG_OK;
        rst_pars->reg_mode             = REG_CURRENT;
        rst_pars->inv_reg_period_iters = 1.0 / (REG_float)reg_signal->reg_period_iters;
        rst_pars->reg_period           = reg_signal->reg_period;

        // Signal to real-time regMgrSignalPrepareRT() to switch to use next RST pars

        mgr_rst_pars->is_next_ready = true;

        // Copy the newly initialised RST parameter structure into reg_signal for debugging

        *reg_signal_last_rst_pars = *rst_pars;
    }
    else // Actuation is VOLTAGE_REF or FIRING_REF
    {
        // Only attempt to initialise RST parameters if regulation for this signal is enabled

        if(reg_signal->regulation == REG_ENABLED)
        {
            struct REG_rst manual;

            // Prepare structure with manual RST coefficients

            memcpy(manual.r, manual_r, REG_NUM_RST_COEFFS * sizeof(REG_float));  // On Mac, gcc returns an error compiling sizeof(manual_r)
            memcpy(manual.s, manual_s, REG_NUM_RST_COEFFS * sizeof(REG_float));
            memcpy(manual.t, manual_t, REG_NUM_RST_COEFFS * sizeof(REG_float));

            // if pure_delay_periods is zero then calculate it

            if(pure_delay_periods <= 0.0)
            {
                pure_delay_periods = (reg_mgr->sim_pc_pars.act_delay_iters + reg_mgr->sim_pc_pars.rsp_delay_iters +
                                      reg_signal->meas.delay_iters[reg_signal->meas.reg_select]) / (REG_float)reg_period_iters;
            }

            // if new RST parameters are valid, the prepare for regulation error calculation

            if(regRstInit(rst_pars, reg_period_iters, reg_signal->reg_period, load_pars,
                          auxpole1_hz, auxpoles2_hz, auxpoles2_z, auxpole4_hz, auxpole5_hz,
                          pure_delay_periods, track_delay_periods, reg_mode, &manual) != REG_FAULT)
            {
                // Calculate ref_advance

                rst_pars->ref_advance = rst_pars->track_delay_periods * rst_pars->reg_period - reg_signal->meas.delay_iters[reg_signal->meas.reg_select] * reg_mgr->iter_period;

                // Set ref_delay to equal the track_delay by default

                rst_pars->ref_delay_periods = rst_pars->track_delay_periods;

                // If reg error is to be calculated at the regulation rate

                if(regMgrVarP(reg_mgr, REG_ERR_RATE) == REG_ERR_RATE_REGULATION)
                {
                    // Regulation error will use the regulation signal (so ref_delay = track_delay)

                    rst_pars->reg_err_meas_select = reg_signal->meas.reg_select;
                }
                else // reg error is to be calculated at the measurement rate
                {
                    // Ideallhy we want to calculate the reg error using the unfiltered measurement

                    rst_pars->reg_err_meas_select = REG_MEAS_UNFILTERED;

                    rst_pars->ref_delay_periods += (reg_signal->meas.delay_iters[REG_MEAS_UNFILTERED] -
                                                    reg_signal->meas.delay_iters[reg_signal->meas.reg_select]) /
                                                    (REG_float)reg_period_iters;

                    // However, if ref_delay is less than 1 period then unfiltered measurement cannot be used

                    if(rst_pars->ref_delay_periods < 1.0)
                    {
                        // This condition can only be true when meas.reg_select is FILTERED and the solution
                        // is to use the filtered measurement for the reg error calculation. 

                        rst_pars->reg_err_meas_select = REG_MEAS_FILTERED;
                        rst_pars->ref_delay_periods   = rst_pars->track_delay_periods;
                    }
                }

                // Signal to real-time regMgrSignalPrepareRT() to switch to use next RST pars

                mgr_rst_pars->is_next_ready = true;
            }
            else
            {
                rst_pars->ref_advance       = 0.0;
                rst_pars->ref_delay_periods = 0.0;
            }

            // Copy the newly initialised RST parameter structure into reg_signal for debugging

            *reg_signal_last_rst_pars = *rst_pars;
        }
    }
}



static void regMgrParsWithMask(struct REG_mgr *reg_mgr, uint32_t par_groups_mask)
{
    uint32_t              i;
    uint32_t              load_select;
    uint32_t              load_test_select;
    uint32_t              test_par_groups_mask = par_groups_mask;
    struct REG_pars_meta *par_meta;

    // Update load_select and load_test_select if they are supplied by calling program and if they are valid

    if(reg_mgr->pars.u.values.load_select != REG_PAR_NOT_USED)
    {
        load_select = *((uint32_t*)reg_mgr->pars.u.values.load_select);

        if(load_select < REG_NUM_LOADS)
        {
            reg_mgr->par_values.load_select[0] = load_select;
        }

        if(reg_mgr->pars.u.values.load_test_select != REG_PAR_NOT_USED)
        {
            load_test_select = *((uint32_t*)reg_mgr->pars.u.values.load_test_select);

            if(load_test_select < REG_NUM_LOADS)
            {
                reg_mgr->par_values.load_test_select[0] = load_test_select;
            }
        }
    }

    load_select      = reg_mgr->par_values.load_select[0];
    load_test_select = reg_mgr->par_values.load_test_select[0];

    // Scan all active parameters for changes

    par_meta = &reg_mgr->pars.meta[0];

    for(i = 0 ; i < REG_NUM_PARS ; i++, par_meta++)
    {
        uint32_t  flags;
        uint32_t  groups;
        char      *value_src = (char*)reg_mgr->pars.u.value[i];

        if(value_src != (char *)REG_PAR_NOT_USED)
        {
            flags  = par_meta->flags;
            groups = par_meta->groups;

            // Skip parameters that must be ignored or are not relevant

            if((reg_mgr->reg_mode     == REG_NONE    || (flags & REG_PAR_FLAG_MODE_NONE_ONLY) == 0) &&
               (reg_mgr->b.regulation == REG_ENABLED || (flags & REG_PAR_FLAG_FIELD_REG     ) == 0) &&
               (reg_mgr->i.regulation == REG_ENABLED || (flags & REG_PAR_FLAG_CURRENT_REG   ) == 0) &&
               (reg_mgr->v.regulation == REG_ENABLED || (flags & REG_PAR_FLAG_VOLTAGE_REG   ) == 0))
            {
                char   *value_dest = (char*)reg_mgr->pars.copy_of_value[i];
                size_t size_in_bytes = par_meta->size_in_bytes;

                // If parameter is an array based on load select then point to scalar value addressed by load_select

                if((flags & REG_PAR_FLAG_LOAD_SELECT) != 0)
                {
                    value_src += load_select * size_in_bytes;
                }

                // If parameter value has changed

                if(memcmp(value_dest,value_src,size_in_bytes) != 0)
                {
                    // Save the changed value and set groups mask for this parameter

                    memcpy(value_dest,value_src,size_in_bytes);

                    par_groups_mask |= groups;
                }

                // If parameter is an array based on load select and it is flagged as being a test parameter,
                // then copy scalar value addressed by load_test_select if it has changed.

                if((flags & (REG_PAR_FLAG_LOAD_SELECT|REG_PAR_FLAG_TEST_PAR)) == (REG_PAR_FLAG_LOAD_SELECT|REG_PAR_FLAG_TEST_PAR))
                {
                    value_src   = (char*)reg_mgr->pars.u.value[i] + load_test_select * size_in_bytes;
                    value_dest += size_in_bytes;

                    // If parameter value has changed

                    if(memcmp(value_dest,value_src,size_in_bytes) != 0)
                    {
                        // Save the changed value and set flags for this parameter

                        memcpy(value_dest,value_src,size_in_bytes);

                        test_par_groups_mask |= groups;
                    }
                }
            }
        }
    }

    // Check every parameter flag in hierarchical order

    // REG_PAR_PC_SIM

    if((par_groups_mask & REG_PAR_GROUP_PC_SIM) != 0)
    {
        regSimPcInit(          &reg_mgr->sim_pc_pars,
                                reg_mgr->iter_period,
                                reg_mgr->par_values.pc_act_delay_iters[0],
                                reg_mgr->par_values.pc_sim_bandwidth[0],
                                reg_mgr->par_values.pc_sim_z[0],
                                reg_mgr->par_values.pc_sim_tau_zero[0],
                                reg_mgr->par_values.pc_sim_den,
                                reg_mgr->par_values.pc_sim_num);
    }

    // REG_PAR_PC_SIM_NOISE_AND_TONE

    if((par_groups_mask & REG_PAR_GROUP_PC_SIM_NOISE_AND_TONE) != 0)
    {
        regMeasSetNoiseAndTone(&reg_mgr->sim_pc_noise_and_tone,
                                reg_mgr->par_values.pc_sim_noise_pp[0],
                                reg_mgr->par_values.pc_sim_tone_pp[0],
                                reg_mgr->par_values.pc_sim_tone_period_iters[0]);
    }

    // REG_PAR_GROUP_INVERT_LIMITS

    if((par_groups_mask & REG_PAR_GROUP_INVERT_LIMITS) != 0)
    {
        regLimMeasInvert(      &reg_mgr->i.lim_meas,
                                reg_mgr->par_values.limits_invert[0]);
        regLimMeasInvert(      &reg_mgr->b.lim_meas,
                                reg_mgr->par_values.limits_invert[0]);
        regLimRefInvert(       &reg_mgr->b.lim_ref,
                                reg_mgr->par_values.limits_invert[0]);
        regLimRefInvert(       &reg_mgr->i.lim_ref,
                                reg_mgr->par_values.limits_invert[0]);
        regLimRefInvert(       &reg_mgr->v.lim_ref,
                                reg_mgr->par_values.limits_invert[0]);
    }

    // REG_PAR_GROUP_V_LIMITS_REF

    if((par_groups_mask & REG_PAR_GROUP_V_LIMITS_REF) != 0)
    {
        regLimVrefInit(        &reg_mgr->v.lim_ref,
                                reg_mgr->par_values.limits_v_pos[0],
                                reg_mgr->par_values.limits_v_neg[0],
                                reg_mgr->par_values.limits_v_rate[0],
                                reg_mgr->par_values.limits_v_acceleration[0],
                                reg_mgr->par_values.limits_i_quadrants41,
                                reg_mgr->par_values.limits_v_quadrants41);
    }

    // REG_PAR_GROUP_V_LIMITS_ERR

    if((par_groups_mask & REG_PAR_GROUP_V_LIMITS_ERR) != 0)
    {
        regErrInitLimits(      &reg_mgr->v.err,
                                reg_mgr->par_values.limits_v_err_warning[0],
                                reg_mgr->par_values.limits_v_err_fault[0]);
    }

    // REG_PAR_GROUP_I_LIMITS_MEAS (reg mode NONE only)

    if((par_groups_mask & REG_PAR_GROUP_I_LIMITS_MEAS) != 0)
    {
        regLimMeasInit(        &reg_mgr->i.lim_meas,
                                reg_mgr->par_values.limits_i_pos[0],
                                reg_mgr->par_values.limits_i_neg[0],
                                reg_mgr->par_values.limits_i_low[0],
                                reg_mgr->par_values.limits_i_zero[0]);
    }

    // REG_PAR_GROUP_I_LIMITS_REF

    if((par_groups_mask & REG_PAR_GROUP_I_LIMITS_REF) != 0)
    {
        regLimRefInit (        &reg_mgr->i.lim_ref,
                                reg_mgr->par_values.limits_i_pos[0],
                                reg_mgr->par_values.limits_i_min[0],
                                reg_mgr->par_values.limits_i_neg[0],
                                reg_mgr->par_values.limits_i_rate[0],
                                reg_mgr->par_values.limits_i_acceleration[0],
                                reg_mgr->par_values.limits_i_closeloop[0]);
    }

    // REG_PAR_GROUP_I_LIMITS_ERR

    if((par_groups_mask & REG_PAR_GROUP_I_LIMITS_ERR) != 0)
    {
        regErrInitLimits(      &reg_mgr->i.err,
                                reg_mgr->par_values.limits_i_err_warning[0],
                                reg_mgr->par_values.limits_i_err_fault[0]);
    }

    // REG_PAR_GROUP_B_LIMITS_MEAS (reg mode NONE only)

    if((par_groups_mask & REG_PAR_GROUP_B_LIMITS_MEAS) != 0)
    {
        regLimMeasInit(        &reg_mgr->b.lim_meas,
                                reg_mgr->par_values.limits_b_pos[0],
                                reg_mgr->par_values.limits_b_neg[0],
                                reg_mgr->par_values.limits_b_low[0],
                                reg_mgr->par_values.limits_b_zero[0]);
    }

    // REG_PAR_GROUP_B_LIMITS_REF

    if((par_groups_mask & REG_PAR_GROUP_B_LIMITS_REF) != 0)
    {
        regLimRefInit (        &reg_mgr->b.lim_ref,
                                reg_mgr->par_values.limits_b_pos[0],
                                reg_mgr->par_values.limits_b_min[0],
                                reg_mgr->par_values.limits_b_neg[0],
                                reg_mgr->par_values.limits_b_rate[0],
                                reg_mgr->par_values.limits_b_acceleration[0],
                                reg_mgr->par_values.limits_b_closeloop[0]);
    }

    // REG_PAR_GROUP_B_LIMITS_ERR

    if((par_groups_mask & REG_PAR_GROUP_B_LIMITS_ERR) != 0)
    {
        regErrInitLimits(      &reg_mgr->b.err,
                                reg_mgr->par_values.limits_b_err_warning[0],
                                reg_mgr->par_values.limits_b_err_fault[0]);
    }

    // REG_PAR_GROUP_I_LIMITS_RMS

    if((par_groups_mask & REG_PAR_GROUP_I_LIMITS_RMS) != 0)
    {
        regLimRmsInit(         &reg_mgr->lim_i_rms,
                                reg_mgr->par_values.limits_i_rms_warning[0],
                                reg_mgr->par_values.limits_i_rms_fault[0],
                                reg_mgr->par_values.limits_i_rms_tc[0],
                                reg_mgr->iter_period);
    }

    // REG_PAR_GROUP_I_LIMITS_RMS_LOAD

    if((par_groups_mask & REG_PAR_GROUP_I_LIMITS_RMS_LOAD) != 0)
    {
        regLimRmsInit(         &reg_mgr->lim_i_rms_load,
                                reg_mgr->par_values.limits_i_rms_load_warning[0],
                                reg_mgr->par_values.limits_i_rms_load_fault[0],
                                reg_mgr->par_values.limits_i_rms_load_tc[0],
                                reg_mgr->iter_period);
    }

    // REG_PAR_GROUP_I_MEAS_REG_SELECT

    if((par_groups_mask & REG_PAR_GROUP_I_MEAS_REG_SELECT) != 0)
    {
        reg_mgr->i.meas.reg_select = reg_mgr->par_values.meas_i_reg_select[0];
    }

    // REG_PAR_GROUP_I_MEAS_FILTER (reg mode NONE only)

    if((par_groups_mask & REG_PAR_GROUP_I_MEAS_FILTER) != 0)
    {
        regMeasFilterInit(     &reg_mgr->i.meas,
                                reg_mgr->par_values.meas_i_fir_lengths,
                                reg_mgr->par_values.ireg_period_iters[0],
                                reg_mgr->par_values.limits_i_pos[0],
                                reg_mgr->par_values.limits_i_neg[0],
                                reg_mgr->par_values.meas_i_delay_iters[0]);
    }

    // REG_PAR_GROUP_B_MEAS_REG_SELECT

    if((par_groups_mask & REG_PAR_GROUP_B_MEAS_REG_SELECT) != 0)
    {
        reg_mgr->b.meas.reg_select = reg_mgr->par_values.meas_b_reg_select[0];
    }

    // REG_PAR_GROUP_B_MEAS_FILTER (reg mode NONE only and field regulation ENABLED)

    if((par_groups_mask & REG_PAR_GROUP_B_MEAS_FILTER) != 0 && reg_mgr->b.regulation == REG_ENABLED)
    {
        regMeasFilterInit(     &reg_mgr->b.meas,
                                reg_mgr->par_values.meas_b_fir_lengths,
                                reg_mgr->par_values.breg_period_iters[0],
                                reg_mgr->par_values.limits_b_pos[0],
                                reg_mgr->par_values.limits_b_neg[0],
                                reg_mgr->par_values.meas_b_delay_iters[0]);
    }

    // REG_PAR_GROUP_MEAS_SIM_DELAYS (reg mode NONE only)

    if((par_groups_mask & REG_PAR_GROUP_MEAS_SIM_DELAYS) != 0)
    {
        regDelayInitDelay(     &reg_mgr->b.sim.meas_delay,
                                reg_mgr->par_values.pc_act_delay_iters[0] + reg_mgr->par_values.meas_b_delay_iters[0] - 1.0);

        regDelayInitDelay(     &reg_mgr->i.sim.meas_delay,
                                reg_mgr->par_values.pc_act_delay_iters[0] + reg_mgr->par_values.meas_i_delay_iters[0] - 1.0);

        regDelayInitDelay(     &reg_mgr->v.sim.meas_delay,
                                reg_mgr->par_values.pc_act_delay_iters[0] + reg_mgr->par_values.meas_v_delay_iters[0] - 1.0);
    }

    // REG_PAR_GROUP_MEAS_SIM_NOISE_AND_TONE

    if((par_groups_mask & REG_PAR_GROUP_MEAS_SIM_NOISE_AND_TONE) != 0)
    {
        // Current and field measurement simulations have noise and tone

        regMeasSetNoiseAndTone(&reg_mgr->b.sim.noise_and_tone,
                                reg_mgr->par_values.meas_b_sim_noise_pp[0],
                                reg_mgr->par_values.meas_b_sim_tone_pp[0],
                                reg_mgr->par_values.meas_sim_tone_period_iters[0]);

        regMeasSetNoiseAndTone(&reg_mgr->i.sim.noise_and_tone,
                                reg_mgr->par_values.meas_i_sim_noise_pp[0],
                                reg_mgr->par_values.meas_i_sim_tone_pp[0],
                                reg_mgr->par_values.meas_sim_tone_period_iters[0]);

        // Voltage measurement simulation only had noise and no tone

        regMeasSetNoiseAndTone(&reg_mgr->v.sim.noise_and_tone,
                                reg_mgr->par_values.meas_v_sim_noise_pp[0],
                                0.0,
                                0);
    }

    // REG_PAR_GROUP_LOAD

    if((par_groups_mask & REG_PAR_GROUP_LOAD) != 0)
    {
        regLoadInit(           &reg_mgr->load_pars,
                                reg_mgr->par_values.load_ohms_ser[0],
                                reg_mgr->par_values.load_ohms_par[0],
                                reg_mgr->par_values.load_ohms_mag[0],
                                reg_mgr->par_values.load_henrys[0],
                                reg_mgr->par_values.load_gauss_per_amp[0]);
    }

    // REG_PAR_GROUP_LOAD_SAT

    if((par_groups_mask & REG_PAR_GROUP_LOAD_SAT) != 0)
    {
        regLoadInitSat(        &reg_mgr->load_pars,
                                reg_mgr->par_values.load_henrys_sat[0],
                                reg_mgr->par_values.load_i_sat_start[0],
                                reg_mgr->par_values.load_i_sat_end[0]);
    }

    // REG_PAR_GROUP_LOAD_SIM

    if((par_groups_mask & REG_PAR_GROUP_LOAD_SIM) != 0)
    {
        regSimLoadInit(        &reg_mgr->sim_load_pars,
                               &reg_mgr->load_pars,
                                reg_mgr->par_values.load_sim_tc_error[0],
                                reg_mgr->iter_period);
    }

    // REG_PAR_GROUP_VREG

    if((test_par_groups_mask & REG_PAR_GROUP_VREG) != 0)
    {

    // Include init function for voltage regulation and output filter simulation

    }

    // REG_PAR_GROUP_IREG

    if((par_groups_mask & REG_PAR_GROUP_IREG) != 0)
    {
        // Loop until updated parameters have been accepted. This may take one iteration period
        // before the real-time thread/interrupt executes and processes a pending set of RST parameters.
        // There is no timeout so if the interrupt blocks, it will back the background thread.

        while(reg_mgr->i.op_rst_pars.is_next_ready == true);

        regMgrRstInit(         reg_mgr,
                                REG_CURRENT,
                                REG_OPERATIONAL_RST_PARS,
                                reg_mgr->par_values.ireg_period_iters[0],
                                reg_mgr->par_values.ireg_auxpole1_hz [0],
                                reg_mgr->par_values.ireg_auxpoles2_hz[0],
                                reg_mgr->par_values.ireg_auxpoles2_z [0],
                                reg_mgr->par_values.ireg_auxpole4_hz [0],
                                reg_mgr->par_values.ireg_auxpole5_hz [0],
                                reg_mgr->par_values.ireg_pure_delay_periods [0],
                                reg_mgr->par_values.ireg_track_delay_periods[0],
                                reg_mgr->par_values.ireg_r,
                                reg_mgr->par_values.ireg_s,
                                reg_mgr->par_values.ireg_t);
    }

    // REG_PAR_GROUP_BREG

    if((par_groups_mask & REG_PAR_GROUP_BREG) != 0)
    {
        // Loop until updated parameters have been accepted. This may take one iteration period
        // before the real-time thread/interrupt executes and processes a pending set of RST parameters
        // There is no timeout so if the interrupt blocks, it will back the background thread.

        while(reg_mgr->b.op_rst_pars.is_next_ready == true);

        regMgrRstInit(         reg_mgr,
                                REG_FIELD,
                                REG_OPERATIONAL_RST_PARS,
                                reg_mgr->par_values.breg_period_iters[0],
                                reg_mgr->par_values.breg_auxpole1_hz [0],
                                reg_mgr->par_values.breg_auxpoles2_hz[0],
                                reg_mgr->par_values.breg_auxpoles2_z [0],
                                reg_mgr->par_values.breg_auxpole4_hz [0],
                                reg_mgr->par_values.breg_auxpole5_hz [0],
                                reg_mgr->par_values.breg_pure_delay_periods [0],
                                reg_mgr->par_values.breg_track_delay_periods[0],
                                reg_mgr->par_values.breg_r,
                                reg_mgr->par_values.breg_s,
                                reg_mgr->par_values.breg_t);
    }

    // REG_PAR_GROUP_LOAD_TEST

    if((test_par_groups_mask & REG_PAR_GROUP_LOAD_TEST) != 0)
    {
        regLoadInit(           &reg_mgr->load_pars_test,
                                reg_mgr->par_values.load_ohms_ser[1],
                                reg_mgr->par_values.load_ohms_par[1],
                                reg_mgr->par_values.load_ohms_mag[1],
                                reg_mgr->par_values.load_henrys[1],
                                reg_mgr->par_values.load_gauss_per_amp[1]);
    }

    // REG_PAR_GROUP_LOAD_SAT_TEST

    if((test_par_groups_mask & REG_PAR_GROUP_LOAD_SAT_TEST) != 0)
    {
        regLoadInitSat(        &reg_mgr->load_pars_test,
                                reg_mgr->par_values.load_henrys_sat[1],
                                reg_mgr->par_values.load_i_sat_start[1],
                                reg_mgr->par_values.load_i_sat_end[1]);
    }

    // REG_PAR_GROUP_IREG_TEST

    if((test_par_groups_mask & REG_PAR_GROUP_IREG_TEST) != 0)
    {
        // Loop until updated parameters have been accepted. This may take one iteration period
        // before the real-time thread/interrupt executes and processes a pending set of RST parameters
        // There is no timeout so if the interrupt blocks, it will back the background thread.

        while(reg_mgr->i.test_rst_pars.is_next_ready == true);

        regMgrRstInit(         reg_mgr,
                                REG_CURRENT,
                                REG_TEST_RST_PARS,
                                reg_mgr->par_values.ireg_period_iters[0],  // Test parameters use operation period always
                                reg_mgr->par_values.ireg_auxpole1_hz[1],
                                reg_mgr->par_values.ireg_auxpoles2_hz[1],
                                reg_mgr->par_values.ireg_auxpoles2_z[1],
                                reg_mgr->par_values.ireg_auxpole4_hz[1],
                                reg_mgr->par_values.ireg_auxpole5_hz[1],
                                reg_mgr->par_values.ireg_pure_delay_periods[1],
                                reg_mgr->par_values.ireg_track_delay_periods[1],
                                reg_mgr->par_values.ireg_test_r,
                                reg_mgr->par_values.ireg_test_s,
                                reg_mgr->par_values.ireg_test_t);
    }

    // REG_PAR_GROUP_BREG_TEST

    if((test_par_groups_mask & REG_PAR_GROUP_BREG_TEST) != 0)
    {
        // Loop until updated parameters have been accepted. This may take one iteration period
        // before the real-time thread/interrupt executes and processes a pending set of RST parameters
        // There is no timeout so if the interrupt blocks, it will back the background thread.

        while(reg_mgr->b.test_rst_pars.is_next_ready == true);

        regMgrRstInit(         reg_mgr,
                                REG_FIELD,
                                REG_TEST_RST_PARS,
                                reg_mgr->par_values.breg_period_iters[0],  // Test parameters use operation period always
                                reg_mgr->par_values.breg_auxpole1_hz[1],
                                reg_mgr->par_values.breg_auxpoles2_hz[1],
                                reg_mgr->par_values.breg_auxpoles2_z[1],
                                reg_mgr->par_values.breg_auxpole4_hz[1],
                                reg_mgr->par_values.breg_auxpole5_hz[1],
                                reg_mgr->par_values.breg_pure_delay_periods[1],
                                reg_mgr->par_values.breg_track_delay_periods[1],
                                reg_mgr->par_values.breg_test_r,
                                reg_mgr->par_values.breg_test_s,
                                reg_mgr->par_values.breg_test_t);
    }
}



void regMgrPars(struct REG_mgr *reg_mgr)
{
    // Call regMgrParsWithMask with mask set to zero so no initialisation functions are run automatically

    regMgrParsWithMask(reg_mgr, 0);
}



void regMgrSimInit(struct REG_mgr *reg_mgr, enum REG_mode reg_mode, REG_float init_meas)
{
    struct REG_mgr_signal *reg_signal = &reg_mgr->i;

    // Initialise all libreg parameters

    regMgrParsWithMask(reg_mgr, 0xFFFFFFFF);

    regMgrSignalPrepareRT(reg_mgr, REG_FIELD,   0, 0);
    regMgrSignalPrepareRT(reg_mgr, REG_CURRENT, 0, 0);

    // Initialise load simulation

    switch(reg_mode)
    {
    case REG_NONE:

        regSimLoadSetVoltage(&reg_mgr->sim_load_pars, &reg_mgr->sim_load_vars, 0.0);
        break;

    case REG_VOLTAGE:

        regSimLoadSetVoltage(&reg_mgr->sim_load_pars, &reg_mgr->sim_load_vars, init_meas * reg_mgr->sim_pc_pars.gain);
        break;

    case REG_CURRENT:

        regSimLoadSetCurrent(&reg_mgr->sim_load_pars, &reg_mgr->sim_load_vars, init_meas);
        break;

    case REG_FIELD:

        regSimLoadSetField(&reg_mgr->sim_load_pars, &reg_mgr->sim_load_vars, init_meas);
        reg_signal = &reg_mgr->b;
        break;
    }

    reg_mgr->v.meas = reg_mgr->sim_load_vars.circuit_voltage;

    reg_mgr->i.meas.signal[REG_MEAS_UNFILTERED] = reg_mgr->sim_load_vars.circuit_current;
    reg_mgr->b.meas.signal[REG_MEAS_UNFILTERED] = reg_mgr->sim_load_vars.magnet_field;

    // Initialise simulated measurement delay histories

    regDelayInitVars(&reg_mgr->b.sim.meas_delay, reg_mgr->b.meas.signal[REG_MEAS_UNFILTERED]);
    regDelayInitVars(&reg_mgr->i.sim.meas_delay, reg_mgr->i.meas.signal[REG_MEAS_UNFILTERED]);
    regDelayInitVars(&reg_mgr->v.sim.meas_delay, reg_mgr->v.meas);

    // Initialise current and field (if enabled) filter histories

    regMgrParsWithMask(reg_mgr, reg_mgr->b.regulation == REG_ENABLED ? REG_PAR_GROUP_B_MEAS_FILTER | REG_PAR_GROUP_I_MEAS_FILTER  : REG_PAR_GROUP_I_MEAS_FILTER);

    // Initialise power converter model history according to the actuation because for CURRENT_REF,
    // the model is used for the current in the load rather than the voltage from the voltage source

    if(regMgrVarP(reg_mgr, PC_ACTUATION) == REG_CURRENT_REF)
    {
        reg_mgr->i.ref_limited = init_meas;
        reg_mgr->sim_load_vars.magnet_current = reg_mgr->i.ref_limited * reg_mgr->sim_pc_pars.gain;
        regSimPcInitHistory(&reg_mgr->sim_pc_pars, &reg_mgr->sim_pc_vars, reg_mgr->sim_load_vars.magnet_current);
    }
    else // Actuation is VOLTAGE_REF or FIRING_REF
    {
        reg_mgr->v.ref_sat     =
        reg_mgr->v.ref_limited =
        reg_mgr->v.ref         = regSimPcInitHistory(&reg_mgr->sim_pc_pars, &reg_mgr->sim_pc_vars, reg_mgr->v.meas);

        reg_signal->ref_openloop = (reg_signal->rst_pars->openloop_reverse.act[0] + reg_signal->rst_pars->openloop_reverse.act[1]) * reg_mgr->v.ref /
                                   (1.0 - reg_signal->rst_pars->openloop_reverse.ref[1]);

        regRstInitHistory(&reg_signal->rst_vars, init_meas, reg_signal->ref_openloop, reg_mgr->v.ref);

        reg_signal->meas.reg = reg_signal->ref = reg_signal->ref_limited = reg_signal->ref_rst = init_meas;

        if(reg_signal->lim_ref.invert_limits == REG_ENABLED)
        {
            reg_mgr->is_openloop = init_meas > -reg_signal->lim_ref.closeloop;
        }
        else
        {
            reg_mgr->is_openloop = init_meas < reg_signal->lim_ref.closeloop;
        }
    }

    regMgrSignalPrepareRT(reg_mgr, reg_mode, 0, 0);
    regMgrModeSetRT(reg_mgr, reg_mode);
    regMgrSimulateRT(reg_mgr, 0.0);
}



void regMgrMeasInit(struct REG_mgr *reg_mgr, struct REG_meas_signal *v_meas_p, struct REG_meas_signal *i_meas_p, struct REG_meas_signal *b_meas_p)
{
    static struct REG_meas_signal null_signal = { 0.0, true };

    reg_mgr->b.input_p = (b_meas_p == NULL ? &null_signal : b_meas_p);;
    reg_mgr->i.input_p = (i_meas_p == NULL ? &null_signal : i_meas_p);;
    reg_mgr->v.input_p = (v_meas_p == NULL ? &null_signal : v_meas_p);
}



// Real-Time Functions

/*!
 * Function to prepare real-time processing each iteration for a regulation signal (Field or Current)
 *
 * This function is called to check if the RST coefficients have been updated by the non-real-time thread
 * and is also able to set the iteration counter when the reg_mode is NONE. This allows synchronous
 * regulation by multiple systems.
 *
 * @param[in,out]     reg_mgr     Pointer to regulation manager structure
 * @param[in]         reg_mode    Regulation signal to process (REG_FIELD or REG_CURRENT)
 * @param[in]         unix_time   Unix_time for this iteration
 * @param[in]         us_time     Microsecond time for this iteration
 */
static void regMgrSignalPrepareRT(struct REG_mgr *reg_mgr, enum REG_mode reg_mode, uint32_t unix_time, uint32_t us_time)
{
    struct REG_rst_pars   *rst_pars;
    struct REG_mgr_signal *reg_signal = reg_mode == REG_FIELD ? &reg_mgr->b : &reg_mgr->i;

    // If the option of regulation for this signal is enabled

    if(reg_signal->regulation == REG_ENABLED)
    {
        // Switch operation RST parameter pointers when switch flag is active

        if(reg_signal->op_rst_pars.is_next_ready)
        {
            rst_pars                       = reg_signal->op_rst_pars.next;
            reg_signal->op_rst_pars.next   = reg_signal->op_rst_pars.active;
            reg_signal->op_rst_pars.active = rst_pars;
            reg_signal->op_rst_pars.is_next_ready = false;
        }

        // Switch test RST parameter pointers when switch flag is active

        if(reg_signal->test_rst_pars.is_next_ready)
        {
            rst_pars                         = reg_signal->test_rst_pars.next;
            reg_signal->test_rst_pars.next   = reg_signal->test_rst_pars.active;
            reg_signal->test_rst_pars.active = rst_pars;
            reg_signal->test_rst_pars.is_next_ready = false;
        }

        // Set rst_pars pointer to link to the active RST parameters (operational or test)

        reg_signal->rst_pars = reg_mgr->reg_rst_source == REG_OPERATIONAL_RST_PARS ? reg_signal->op_rst_pars.active : reg_signal->test_rst_pars.active;

        // Increment iteration counter for this new iteration

        reg_signal->iteration_counter++;

        // Set regulation iteration counter from the time when reg_mode is NONE

        if(reg_mgr->reg_mode == REG_NONE)
        {
            /*!
             * If multiple systems must synchronize their regulation periods, then the regulation
             * period (reg_period_iters * iter_period_us) should divide into 12s exactly.
             * This awk command lists the periods in microseconds from 100us to 10000us (10kHz to 10Hz) that do this:
             *     awk 'BEGIN { for(us=100 ; us<=100000 ; us++) { g = 12000000/us; if (int(g) == g) print us}}'
             */

            reg_signal->iteration_counter = ((((unix_time % 12) * 1000000) + us_time) / reg_mgr->iter_period_us) % reg_signal->reg_period_iters;
        }
    }
}



static void regMgrModeSetNoneOrVoltageRT(struct REG_mgr *reg_mgr, enum REG_mode reg_mode)
{
    if(reg_mode == REG_VOLTAGE)
    {
        // Voltage regulation - Initialise voltage references according to previous regulation mode

        switch(reg_mgr->reg_mode)
        {
            case REG_FIELD:

                reg_mgr->v.ref     = regRstAverageVrefRT(&reg_mgr->b.rst_vars);
                reg_mgr->v.ref_sat = reg_mgr->v.ref;
                break;

            case REG_CURRENT:

                reg_mgr->v.ref     = regRstAverageVrefRT(&reg_mgr->i.rst_vars);
                reg_mgr->v.ref_sat = regLoadVrefSatRT(&reg_mgr->load_pars, reg_mgr->i.rst_vars.meas[0], reg_mgr->v.ref);
                break;

            default:    // NONE

                reg_mgr->v.ref_sat = reg_mgr->v.ref;
                break;
        }

        reg_mgr->v.ref_limited = reg_mgr->v.ref_sat;
    }
    else // REG_NONE
    {
        reg_mgr->v.ref         = 0.0;
        reg_mgr->v.ref_sat     = 0.0;
        reg_mgr->v.ref_limited = 0.0;
    }

    // Clear field and current regulation variables

    reg_mgr->flags.ref_clip      = 0;
    reg_mgr->flags.ref_rate      = 0;

    reg_mgr->reg_signal = &reg_mgr->i;
    reg_mgr->lim_ref    = &reg_mgr->v.lim_ref;

    reg_mgr->ref_advance = reg_mgr->iter_period * (reg_mgr->sim_pc_pars.act_delay_iters + reg_mgr->sim_pc_pars.rsp_delay_iters);
}



static void regMgrModeSetFieldOrCurrentRT(struct REG_mgr *reg_mgr, enum REG_mode reg_mode)
{
    uint32_t                idx;
    struct REG_mgr_signal *reg_signal;
    struct REG_rst_pars    *rst_pars;
    struct REG_rst_vars    *rst_vars;

    // Get points to RST parameters and variables

    reg_signal = reg_mgr->reg_signal = (reg_mode == REG_FIELD ? &reg_mgr->b : &reg_mgr->i);

    rst_pars   =  reg_signal->rst_pars;
    rst_vars   = &reg_signal->rst_vars;

    // If actuation is CURRENT_REF then current regulation is in the power converter

    if(regMgrVarP(reg_mgr, PC_ACTUATION) == REG_CURRENT_REF)
    {
        REG_float meas_reg;

        regMgrModeSetNoneOrVoltageRT(reg_mgr, REG_NONE);

        rst_pars->ref_delay_periods = reg_mgr->ref_advance / reg_mgr->iter_period;

        meas_reg = reg_signal->meas.signal[reg_signal->meas.reg_select];
        reg_signal->ref = reg_signal->ref_delayed = reg_signal->meas.reg = meas_reg;

        for(idx = 0; idx <= REG_RST_HISTORY_MASK; idx++)
        {
            rst_vars->act         [idx] = 0.0;
            rst_vars->meas        [idx] = meas_reg;
            rst_vars->ref         [idx] = meas_reg;
            rst_vars->openloop_ref[idx] = meas_reg;
        }
    }
    else // Actuation is VOLTAGE_REF or FIRING_REF so current or field regulation is in libreg
    {
        reg_mgr->ref_advance = rst_pars->ref_advance;

        reg_signal->is_delayed_ref_available = (regMgrVarP(reg_mgr, REG_ERR_RATE) == REG_ERR_RATE_MEASUREMENT &&
                                                rst_pars->reg_err_meas_select    == REG_MEAS_UNFILTERED);

        regRstInitRefRT(rst_pars, rst_vars, reg_signal->rate.estimate);

        reg_signal->ref = reg_signal->ref_openloop = regRstPrevRefRT(&reg_signal->rst_vars);

        reg_signal->ref_delayed = regRstDelayedRefRT(rst_pars, rst_vars, reg_signal->iteration_counter);
    }
    reg_signal->ref_limited = reg_signal->ref_rst = reg_signal->ref;

    reg_mgr->lim_ref     = &reg_signal->lim_ref;
    reg_mgr->reg_period  = rst_pars->reg_period;
}



static void regMgrResetRegSignalRT(struct REG_mgr_signal *reg_signal)
{
    reg_signal->is_delayed_ref_available = false;
    reg_signal->meas.reg                 = 0.0;
    reg_signal->ref                      = 0.0;
    reg_signal->ref_limited              = 0.0;
    reg_signal->ref_rst                  = 0.0;
    reg_signal->ref_openloop             = 0.0;
    reg_signal->ref_delayed              = 0.0;
    reg_signal->track_delay_periods      = 0.0;
}



void regMgrModeSetRT(struct REG_mgr *reg_mgr, enum REG_mode reg_mode)
{
    // If regulation mode has changed

    if(reg_mode != reg_mgr->reg_mode)
    {
        // Switch on current mode - reset regulation errors

        switch(reg_mgr->reg_mode)
        {
            case REG_NONE:
            case REG_VOLTAGE:

                break;

            case REG_CURRENT:

                regMgrResetRegSignalRT(&reg_mgr->i);
                regErrResetLimitsVarsRT(&reg_mgr->i.err);
                break;

            case REG_FIELD:

                regMgrResetRegSignalRT(&reg_mgr->b);
                regErrResetLimitsVarsRT(&reg_mgr->b.err);
                break;
        }

        // Switch on new mode

        switch(reg_mode)
        {
            case REG_NONE:

                regErrResetLimitsVarsRT(&reg_mgr->v.err);
                regMgrModeSetNoneOrVoltageRT(reg_mgr, reg_mode);
                break;

            case REG_VOLTAGE:

                regMgrModeSetNoneOrVoltageRT(reg_mgr, reg_mode);
                break;

            case REG_CURRENT:
            case REG_FIELD:

                regMgrModeSetFieldOrCurrentRT(reg_mgr, reg_mode);
                break;
        }

        // Store the new regulation mode

        reg_mgr->reg_mode = reg_mode;
    }
}



uint32_t regMgrMeasSetRT(struct REG_mgr *reg_mgr, enum REG_rst_source reg_rst_source,
                         uint32_t unix_time, uint32_t us_time,
                         bool use_sim_meas, bool is_max_abs_err_enabled)
{
    uint32_t iteration_counter;
    REG_float    i_meas_unfiltered;

    // Store parameters for this iteration

    reg_mgr->reg_rst_source         = reg_rst_source;
    reg_mgr->is_max_abs_err_enabled = is_max_abs_err_enabled;

    // Check for new RST parameters and manage iteration counters

    regMgrSignalPrepareRT(reg_mgr, REG_FIELD,   unix_time, us_time);
    regMgrSignalPrepareRT(reg_mgr, REG_CURRENT, unix_time, us_time);

    // Use simulated or real measurements as required

    if(use_sim_meas)
    {
        // Use simulated measurements which are always valid

        reg_mgr->b.input.signal = reg_mgr->b.sim.signal;
        reg_mgr->i.input.signal = reg_mgr->i.sim.signal;
        reg_mgr->v.input.signal = reg_mgr->v.sim.signal;

        reg_mgr->b.input.is_valid = true;
        reg_mgr->i.input.is_valid = true;
        reg_mgr->v.input.is_valid = true;
    }
    else
    {
        // Use real field, current and voltage measurements and measurement statuses supplied by application

        reg_mgr->b.input = *reg_mgr->b.input_p;
        reg_mgr->i.input = *reg_mgr->i.input_p;
        reg_mgr->v.input = *reg_mgr->v.input_p;
    }

    // If regulating CURRENT or FIELD then calculate the delayed reference

    if(reg_mgr->reg_mode == REG_CURRENT || reg_mgr->reg_mode == REG_FIELD)
    {
        // Note that regRstDelayedRefRT requires the iteration counter to run from 1 - reg_period_iters so it is
        // used after incrementing in regMgrSignalRT() and before being wrapped

        reg_mgr->reg_signal->ref_delayed = regRstDelayedRefRT(reg_mgr->reg_signal->rst_pars, &reg_mgr->reg_signal->rst_vars, reg_mgr->reg_signal->iteration_counter);
    }

    // Wrap iteration counters

    if(reg_mgr->i.iteration_counter >= reg_mgr->i.reg_period_iters)
    {
        reg_mgr->i.iteration_counter = 0;
    }

    if(reg_mgr->b.iteration_counter >= reg_mgr->b.reg_period_iters)
    {
        reg_mgr->b.iteration_counter = 0;
    }

    // Prepare to return the iteration counter for the active regulation mode

    if(reg_mgr->reg_mode == REG_CURRENT)
    {
        iteration_counter = reg_mgr->i.iteration_counter;
    }
    else if(reg_mgr->reg_mode == REG_FIELD)
    {
        iteration_counter = reg_mgr->b.iteration_counter;
    }
    else // REG_NONE or REG_VOLTAGE
    {
        iteration_counter = 0;
    }

    // Check voltage measurement

    if(reg_mgr->v.input.is_valid == false)
    {
        // If input voltage measurement is invalid then use voltage source model instead

        reg_mgr->v.meas = reg_mgr->v.err.delayed_ref;
        reg_mgr->v.invalid_input_counter++;
        reg_mgr->v.invalid_seq_counter++;
    }
    else
    {
        reg_mgr->v.meas = reg_mgr->v.input.signal;
        reg_mgr->v.invalid_seq_counter = 0;
    }

    // Check current measurement

    if(reg_mgr->i.input.is_valid == false)
    {
        if(reg_mgr->i.is_delayed_ref_available)
        {
            // If regulating current then use delayed ref adjusted by the regulation error as the measurement

            i_meas_unfiltered = reg_mgr->i.meas.signal[REG_MEAS_UNFILTERED] = reg_mgr->i.ref_delayed - reg_mgr->i.err.err;
        }
        else // otherwise extrapolate previous signal value using the estimated rate of change
        {
            i_meas_unfiltered = reg_mgr->i.meas.signal[REG_MEAS_UNFILTERED] += reg_mgr->i.rate.estimate * reg_mgr->iter_period;
        }

        reg_mgr->i.invalid_input_counter++;
        reg_mgr->i.invalid_seq_counter++;
    }
    else
    {
        i_meas_unfiltered = reg_mgr->i.meas.signal[REG_MEAS_UNFILTERED] = reg_mgr->i.input.signal;
        reg_mgr->i.invalid_seq_counter = 0;
    }

    // Filter the current measurement and prepare to estimate the measurement rate

    regMeasFilterRT(&reg_mgr->i.meas);
    regMeasRateRT  (&reg_mgr->i.rate, reg_mgr->i.meas.signal[REG_MEAS_FILTERED],
                     reg_mgr->i.inv_reg_period, reg_mgr->i.reg_period_iters);

    // Check current measurement and RMS limits

    regLimMeasRT   (&reg_mgr->i.lim_meas,     i_meas_unfiltered);
    regLimMeasRmsRT(&reg_mgr->lim_i_rms,      i_meas_unfiltered);
    regLimMeasRmsRT(&reg_mgr->lim_i_rms_load, i_meas_unfiltered);

    // Update RST history index and store new measurement

    if(reg_mgr->i.iteration_counter == 0)
    {
        regRstIncHistoryIndexRT(&reg_mgr->i.rst_vars);
        reg_mgr->i.rst_vars.meas[reg_mgr->i.rst_vars.history_index] = reg_mgr->i.meas.signal[reg_mgr->i.meas.reg_select];
    }

    // Check field measurement if option of field regulation is ENABLED

    if(reg_mgr->b.regulation == REG_ENABLED)
    {
        if(reg_mgr->b.input.is_valid == false)
        {
            // If regulating field and delayed ref is available then use it to estimate missing field
            // measurement, adjusted by the latest regulation error

            if(reg_mgr->b.is_delayed_ref_available)
            {
                reg_mgr->b.meas.signal[REG_MEAS_UNFILTERED] = reg_mgr->b.ref_delayed - reg_mgr->b.err.err;
            }
            else // otherwise extrapolate previous signal value using the estimated rate of change
            {
                reg_mgr->b.meas.signal[REG_MEAS_UNFILTERED] += reg_mgr->b.rate.estimate * reg_mgr->iter_period;
            }

            reg_mgr->b.invalid_input_counter++;
            reg_mgr->b.invalid_seq_counter++;
        }
        else
        {
            reg_mgr->b.meas.signal[REG_MEAS_UNFILTERED] = reg_mgr->b.input.signal;
            reg_mgr->b.invalid_seq_counter = 0;
        }

        // Filter the field measurement, prepare to estimate measurement rate and apply limits

        regMeasFilterRT(&reg_mgr->b.meas);
        regMeasRateRT  (&reg_mgr->b.rate,reg_mgr->b.meas.signal[REG_MEAS_FILTERED],
                         reg_mgr->b.inv_reg_period, reg_mgr->b.reg_period_iters);
        regLimMeasRT   (&reg_mgr->b.lim_meas, reg_mgr->b.meas.signal[REG_MEAS_UNFILTERED]);

        // Update RST history index and store new measurement

        if(reg_mgr->b.iteration_counter == 0)
        {
            regRstIncHistoryIndexRT(&reg_mgr->b.rst_vars);

            reg_mgr->b.rst_vars.meas[reg_mgr->b.rst_vars.history_index] = reg_mgr->b.meas.signal[reg_mgr->b.meas.reg_select];
        }
    }

    // When actuation is VOLTAGE_REF or FIRING_REF and the converter is running, then manage the voltage related limits

    if(regMgrVarP(reg_mgr, PC_ACTUATION) != REG_CURRENT_REF && reg_mgr->reg_mode != REG_NONE)
    {
        // Calculate and check the voltage regulation limits

        regErrCheckLimitsRT(&reg_mgr->v.err, true, reg_mgr->v.err.delayed_ref, reg_mgr->v.meas);

        // Calculate voltage reference limits for the measured current (V limits can depend on current)

        regLimVrefCalcRT(&reg_mgr->v.lim_ref, i_meas_unfiltered);
    }

    return(iteration_counter);
}



static void regMgrSignalRegulateRT(struct REG_mgr *reg_mgr, enum REG_mode reg_mode, REG_float *ref)
{
    struct REG_mgr_signal *reg_signal = reg_mode == REG_FIELD ? &reg_mgr->b : &reg_mgr->i;

    if(reg_signal->regulation == REG_ENABLED)
    {
        // If voltage reference was already calculated, ref will equal NULL

        if(ref == NULL)
        {
            // Save voltage reference in RST actuation history - compensate for magnet saturation for CURRENT regulation

            if(reg_signal->iteration_counter == 0)
            {
                reg_signal->rst_vars.act[reg_signal->rst_vars.history_index] = reg_mode == REG_CURRENT ?
                        regLoadInverseVrefSatRT(&reg_mgr->load_pars, reg_mgr->i.meas.signal[REG_MEAS_UNFILTERED], reg_mgr->v.ref_limited) :
                        reg_mgr->v.ref_limited;
            }
        }
        else
        {
            struct REG_rst_pars * rst_pars = reg_signal->rst_pars;

            if(reg_signal->iteration_counter == 0)
            {
                reg_signal->ref  = *ref;
                reg_signal->meas.reg = reg_signal->meas.signal[reg_signal->meas.reg_select];

                // Apply current reference clip and rate limits

                reg_signal->ref_limited = regLimRefRT(&reg_signal->lim_ref, reg_mgr->reg_period, reg_signal->ref, reg_signal->ref_limited);

                // If Actuation is CURRENT_REF - current regulation is inside the power converter

                if(regMgrVarP(reg_mgr, PC_ACTUATION) == REG_CURRENT_REF)
                {
                    reg_mgr->i.rst_vars.ref[reg_mgr->i.rst_vars.history_index] = reg_signal->ref_limited;
                }
                else // Actuation is VOLTAGE_REF or FIRING_REF - libreg is responsible for the the current or field regulation
                {
                    bool  is_limited;
                    REG_float v_ref;
                    REG_float meas;

                    // Calculate voltage reference using RST algorithm

                    reg_mgr->v.ref = regRstCalcActRT(rst_pars, &reg_signal->rst_vars, reg_signal->ref_limited, reg_mgr->is_openloop);

                    // Calculate magnet saturation compensation when regulating current only

                    reg_mgr->v.ref_sat = (reg_mode == REG_CURRENT ?
                                       regLoadVrefSatRT(&reg_mgr->load_pars, reg_mgr->i.meas.signal[REG_MEAS_UNFILTERED], reg_mgr->v.ref) :
                                       reg_mgr->v.ref);

                    // Apply voltage reference clip and rate limits

                    reg_mgr->v.ref_limited = regLimRefRT(&reg_mgr->v.lim_ref, reg_mgr->reg_period, reg_mgr->v.ref_sat, reg_mgr->v.ref_limited);

                    // If voltage reference has been clipped

                    is_limited = (reg_mgr->v.lim_ref.flags.clip || reg_mgr->v.lim_ref.flags.rate);

                    if(is_limited)
                    {
                        // Back calculate the new v_ref before the saturation compensation when regulating current only

                        v_ref = (reg_mode == REG_CURRENT ? 
                                 regLoadInverseVrefSatRT(&reg_mgr->load_pars, reg_mgr->i.meas.signal[REG_MEAS_UNFILTERED], reg_mgr->v.ref_limited) :
                                 reg_mgr->v.ref_limited);

                        // Mark current reference as rate limited

                        reg_signal->lim_ref.flags.rate = REG_ENABLED;
                    }
                    else
                    {
                        v_ref = reg_mgr->v.ref;
                    }

                    reg_mgr->flags.ref_clip = reg_signal->lim_ref.flags.clip;
                    reg_mgr->flags.ref_rate = reg_signal->lim_ref.flags.rate;

                    reg_signal->track_delay_periods = regRstTrackDelayRT(&reg_mgr->reg_signal->rst_vars);

                    // Back calculate new current reference to keep RST histories balanced

                    regRstCalcRefRT(rst_pars, &reg_signal->rst_vars, v_ref, is_limited, reg_mgr->is_openloop);

                    reg_signal->ref_rst      = reg_signal->rst_vars.ref         [reg_signal->rst_vars.history_index];
                    reg_signal->ref_openloop = reg_signal->rst_vars.openloop_ref[reg_signal->rst_vars.history_index];

                    // Switch between open/closed loop according to closeloop threshold

                    meas = (reg_signal->lim_ref.invert_limits == REG_ENABLED ? -reg_signal->meas.reg : reg_signal->meas.reg);

                    if(reg_mgr->is_openloop)
                    {
                        if(meas > reg_signal->lim_ref.closeloop)
                        {
                            reg_mgr->is_openloop = false;

                            *ref = reg_signal->ref_limited = reg_signal->ref_rst;
                        }
                        else
                        {
                            *ref = reg_signal->ref_openloop;
                        }
                    }
                    else
                    {
                        if(meas < reg_signal->lim_ref.closeloop)
                        {
                            reg_mgr->is_openloop = true;

                            *ref = reg_signal->ref_limited = reg_signal->ref_openloop;
                        }
                        else
                        {
                            *ref = reg_signal->ref_rst;
                        }
                    }

                }
            }

            // Monitor regulation error using the delayed reference

            if(regMgrVarP(reg_mgr, REG_ERR_RATE) == REG_ERR_RATE_MEASUREMENT || reg_signal->iteration_counter == 0)
            {
                regErrCheckLimitsRT(&reg_signal->err, 
                                    reg_mgr->is_max_abs_err_enabled,
                                    reg_signal->ref_delayed,
                                    reg_signal->meas.signal[rst_pars->reg_err_meas_select]);
            }
        }
    }
}



void regMgrRegulateRT(struct REG_mgr *reg_mgr, REG_float *ref)
{
    switch(reg_mgr->reg_mode)
    {
    case REG_NONE:

        break;

    case REG_VOLTAGE:

        // *ref is a voltage reference so just apply limits

        reg_mgr->v.ref_sat = reg_mgr->v.ref = *ref;

        reg_mgr->v.ref_limited = regLimRefRT(&reg_mgr->v.lim_ref, reg_mgr->iter_period, reg_mgr->v.ref, reg_mgr->v.ref_limited);

        reg_mgr->flags.ref_clip = reg_mgr->v.lim_ref.flags.clip;
        reg_mgr->flags.ref_rate = reg_mgr->v.lim_ref.flags.rate;

        *ref = reg_mgr->v.ref_limited;

        // Keep RST act history up to date for field and current regulation

        regMgrSignalRegulateRT(reg_mgr, REG_FIELD,   NULL);
        regMgrSignalRegulateRT(reg_mgr, REG_CURRENT, NULL);
        break;

    case REG_CURRENT:

        regMgrSignalRegulateRT(reg_mgr, REG_CURRENT, ref);
        regMgrSignalRegulateRT(reg_mgr, REG_FIELD,   NULL);
        break;

    case REG_FIELD:

        regMgrSignalRegulateRT(reg_mgr, REG_FIELD,   ref);
        regMgrSignalRegulateRT(reg_mgr, REG_CURRENT, NULL);
        break;
    }
}



static REG_float regMgrSimulationQuantization(REG_float signal, REG_float quantization)
{
    if(quantization > 0.0)
    {
        signal = quantization * nearbyintf(signal / quantization);
    }

    return(signal);
}



void regMgrSimulateRT(struct REG_mgr *reg_mgr, REG_float v_perturbation)
{
    REG_float v_circ;      // Simulated v_circuit without PC ACT_DELAY

    // If Actuation is CURRENT_REF

    if(regMgrVarP(reg_mgr, PC_ACTUATION) == REG_CURRENT_REF)
    {
        // Use the power converter model as the current source model and assume that all the circuit current passes through the magnet
        // i.e. assume ohms_par is large - if this is not true then the simulation will not be accurate

        reg_mgr->sim_load_vars.circuit_current = regSimPcRT(&reg_mgr->sim_pc_pars, &reg_mgr->sim_pc_vars, reg_mgr->i.ref_limited) +
                                                 regMeasNoiseAndToneRT(&reg_mgr->sim_pc_noise_and_tone);

        // Derive the circuit voltage using V = I.R + L(I) dI/dt
        // Note: reg_mgr->sim_load_vars.magnet_current contains current from previous iteration so it is used to calculate dI/dt

        reg_mgr->sim_load_vars.circuit_voltage = reg_mgr->sim_load_vars.circuit_current * reg_mgr->sim_load_pars.load_pars.ohms +
                                                 reg_mgr->sim_load_pars.load_pars.henrys *
                                                 regLoadSatFactorRT(&reg_mgr->sim_load_pars.load_pars,reg_mgr->sim_load_vars.circuit_voltage) *
                                                 (reg_mgr->sim_load_vars.circuit_current - reg_mgr->sim_load_vars.magnet_current) / reg_mgr->iter_period;

        reg_mgr->sim_load_vars.magnet_current = reg_mgr->sim_load_vars.circuit_current;

        // Derive the simulated magnetic field

        reg_mgr->sim_load_vars.magnet_field = regLoadCurrentToFieldRT(&reg_mgr->sim_load_pars.load_pars, reg_mgr->sim_load_vars.magnet_current);
    }
    else // Actuation is VOLTAGE_REF or FIRING_REF
    {
        // Simulate circuit voltage based from voltage reference using voltage source model

        if(regMgrVarP(reg_mgr, PC_ACTUATION) == REG_VOLTAGE_REF)
        {
            v_circ = regSimPcRT(&reg_mgr->sim_pc_pars, &reg_mgr->sim_pc_vars, regMgrSimulationQuantization(reg_mgr->v.ref_limited, regMgrVarP(reg_mgr, PC_SIM_QUANTIZATION)));
        }
        else // Simulate circuit voltage from firing reference using output filter model
        {
            v_circ = regSimPcRT(&reg_mgr->sim_pc_pars, &reg_mgr->sim_pc_vars, regMgrSimulationQuantization(reg_mgr->v.ref_limited, regMgrVarP(reg_mgr, PC_SIM_QUANTIZATION)));
        }

        // Combine voltage perturbation and noise and tone with simulated circuit voltage

        v_circ += v_perturbation + regMeasNoiseAndToneRT(&reg_mgr->sim_pc_noise_and_tone);

        // Simulate load current and field in response to v_circuit plus the perturbation, also without taking into account PC ACT_DELAY

        regSimLoadRT(&reg_mgr->sim_load_pars, &reg_mgr->sim_load_vars, reg_mgr->sim_pc_pars.is_pc_undersampled, v_circ);
    }

    // Use delays to estimate the measurement of the magnet's field and the circuit's current and voltage

    reg_mgr->b.sim.signal = regDelaySignalRT(&reg_mgr->b.sim.meas_delay,
                                              reg_mgr->sim_load_vars.magnet_field,
                                              reg_mgr->sim_pc_pars.is_pc_undersampled && reg_mgr->sim_load_pars.is_load_undersampled);

    reg_mgr->i.sim.signal = regDelaySignalRT(&reg_mgr->i.sim.meas_delay,
                                              reg_mgr->sim_load_vars.circuit_current,
                                              reg_mgr->sim_pc_pars.is_pc_undersampled && reg_mgr->sim_load_pars.is_load_undersampled);

    reg_mgr->v.sim.signal = regDelaySignalRT(&reg_mgr->v.sim.meas_delay,
                                              reg_mgr->sim_load_vars.circuit_voltage,
                                              reg_mgr->sim_pc_pars.is_pc_undersampled);

    // Store simulated voltage measurement without noise as the delayed ref for the v_err calculation

    reg_mgr->v.err.delayed_ref = reg_mgr->v.sim.signal;

    // Simulate noise and tone on simulated measurement of the magnet's field and the circuit's current and voltage

    reg_mgr->b.sim.signal += regMeasNoiseAndToneRT(&reg_mgr->b.sim.noise_and_tone);
    reg_mgr->i.sim.signal += regMeasNoiseAndToneRT(&reg_mgr->i.sim.noise_and_tone);
    reg_mgr->v.sim.signal += regMeasNoiseAndToneRT(&reg_mgr->v.sim.noise_and_tone);

    // Simulation ADC quantisation

    reg_mgr->b.sim.signal = regMgrSimulationQuantization(reg_mgr->b.sim.signal, regMgrVarP(reg_mgr, MEAS_B_SIM_QUANTIZATION));
    reg_mgr->i.sim.signal = regMgrSimulationQuantization(reg_mgr->i.sim.signal, regMgrVarP(reg_mgr, MEAS_I_SIM_QUANTIZATION));
    reg_mgr->v.sim.signal = regMgrSimulationQuantization(reg_mgr->v.sim.signal, regMgrVarP(reg_mgr, MEAS_V_SIM_QUANTIZATION));
}

// EOF
