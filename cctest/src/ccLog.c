/*---------------------------------------------------------------------------------------------------------*\
  File:     ccLog.c                                                                     Copyright CERN 2015

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

  Purpose:  ccTest signal logging functions

  Authors:  Quentin.King@cern.ch
\*---------------------------------------------------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>

#include "ccCmds.h"
#include "ccTest.h"
#include "ccFile.h"
#include "ccRun.h"
#include "ccLog.h"
#include "ccFlot.h"
#include "libreg_vars_test.h"


static void ccLogResetSignals(struct cclog *log)
{
    uint32_t sig_idx;

    log->num_samples = 0;

    // Reset analogue signals

    for(sig_idx = 0 ; sig_idx < log->num_ana_signals ; sig_idx++)
    {
        log->ana_sigs[sig_idx].is_enabled     = false;
        log->ana_sigs[sig_idx].num_bad_values = 0;
        log->ana_sigs[sig_idx].time_offset    = 0.0;

        if(log->ana_sigs[sig_idx].buf != NULL)
        {
            free(log->ana_sigs[sig_idx].buf);
        }

        log->ana_sigs[sig_idx].buf = NULL;
    }

    // Reset digital signals if present

    for(sig_idx = 0 ; sig_idx < log->num_dig_signals ; sig_idx++)
    {
        log->dig_sigs[sig_idx].is_enabled = false;

        if(log->dig_sigs[sig_idx].buf != NULL)
        {
            free(log->dig_sigs[sig_idx].buf);
        }

        log->dig_sigs[sig_idx].buf = NULL;
    }
}



static void ccLogEnableAnaSignal(struct cclog_ana_sigs *ana_sig)
{
    // Enable signal

    ana_sig->is_enabled = true;

    // If HTML output enabled then allocate buffer memory

    if(ccpars_global.html_output == REG_ENABLED)
    {
        ana_sig->buf = (float *)calloc(ccpars_global.log_length, sizeof(float));
    }
}



static void ccLogEnableDigSignal(struct cclog_dig_sigs *dig_sig)
{
    // Enable signal

    dig_sig->is_enabled = true;

    // If FLOT output enabled then allocate buffer memory

    if(ccpars_global.html_output == REG_ENABLED)
    {
        // Allocate space for overflow point since flot_index will stop at FLOT_POINTS_MAX

        dig_sig->buf = (uint8_t *)calloc(ccpars_global.log_length, sizeof(uint8_t));
    }
}



static void ccLogStoreSignals(struct cclog *log)
{
    uint32_t sig_idx;
    float    value;

    // Store analogue signals

    for(sig_idx = 0 ; sig_idx < log->num_ana_signals ; sig_idx++)
    {
        struct cclog_ana_sigs *ana_sig = &log->ana_sigs[sig_idx];

        if(ana_sig->is_enabled)
        {
            value = *ana_sig->source;

            // Protect against bad values and count how many occur

            if(!isfinite(value) || fabs(value) > 1.0E6)
            {
                value = 0.0;
                ana_sig->num_bad_values++;
            }

            ana_sig->value = value;

            if(ana_sig->buf != NULL)
            {
                ana_sig->buf[log->last_sample_index] = value;
            }
        }
    }
    
    // Store digital signals if present
    
    for(sig_idx = 0 ; sig_idx < log->num_dig_signals ; sig_idx++)
    {
        struct cclog_dig_sigs *dig_sig = &log->dig_sigs[sig_idx];

        if(dig_sig->is_enabled)
        {
            dig_sig->value = *dig_sig->source;

            if(dig_sig->buf != NULL)
            {
                dig_sig->buf[log->last_sample_index] = dig_sig->value;
            }
        }
    }
}



void ccLogInit(void)
{
    // This function does nothing useful at run time but the compiler checks that all
    // libreg variable macros are valid

    regMgrTestVarMacros();

    // Disable all signals and free log memory

    ccLogResetSignals(&breg_log);
    ccLogResetSignals(&ireg_log);
    ccLogResetSignals(&meas_log);

    // Voltage reference is always enabled when converter actuation is VOLTAGE_REF

    if(ccpars_pc.actuation == REG_VOLTAGE_REF)
    {
        ccLogEnableAnaSignal(&ana_meas_sigs[ANA_V_REF]);
    }

    // Enable additional signals with simulating load

    if(ccpars_global.sim_load == REG_ENABLED)
    {
        // Set time offset for circuit simulation signals - if the voltage source is under-sampled then
        // include the steady ramp delay, otherwise the dynamic response is actually simulated

        ana_meas_sigs[ANA_B_MAGNET ].time_offset =
        ana_meas_sigs[ANA_I_MAGNET ].time_offset =
        ana_meas_sigs[ANA_I_CIRCUIT].time_offset =
        ana_meas_sigs[ANA_V_CIRCUIT].time_offset = reg_mgr.iter_period * (ccpars_pc.act_delay_iters +
                                                  (reg_mgr.sim_pc_pars.is_pc_undersampled == 0 ? 0.0 : reg_mgr.sim_pc_pars.rsp_delay_iters));

        if(ccpars_pc.actuation == REG_VOLTAGE_REF)
        {
            // Voltage source simulation signals

            ccLogEnableAnaSignal(&ana_meas_sigs[ANA_V_REF_LIMITED]);
            ccLogEnableAnaSignal(&ana_meas_sigs[ANA_V_CIRCUIT]);
            ccLogEnableAnaSignal(&ana_meas_sigs[ANA_V_MEAS]);
            ccLogEnableAnaSignal(&ana_meas_sigs[ANA_V_ERR]);
            ccLogEnableAnaSignal(&ana_meas_sigs[ANA_MAX_ABS_V_ERR]);

            ccLogEnableDigSignal(&dig_meas_sigs[DIG_V_REF_CLIP]);
            ccLogEnableDigSignal(&dig_meas_sigs[DIG_V_REF_RATE_CLIP]);

            if(ccpars_limits.v_err_warning > 0.0)
            {
                ccLogEnableDigSignal(&dig_meas_sigs[DIG_V_REG_ERR_WARN]);
            }

            if(ccpars_limits.v_err_fault > 0.0)
            {
                ccLogEnableDigSignal(&dig_meas_sigs[DIG_V_REG_ERR_FLT]);
            }

            if(ccrun.is_breg_enabled)
            {
                ccLogEnableAnaSignal(&ana_breg_sigs[ANA_MEAS_REG]);
                ccLogEnableAnaSignal(&ana_breg_sigs[ANA_REF]);
                ccLogEnableAnaSignal(&ana_breg_sigs[ANA_REF_LIMITED]);
                ccLogEnableAnaSignal(&ana_breg_sigs[ANA_REF_RST]);
                ccLogEnableAnaSignal(&ana_breg_sigs[ANA_TRACK_DELAY]);

                if(ccpars_limits.b_neg[ccpars_load.select] >= 0.0)
                {
                    ccLogEnableAnaSignal(&ana_breg_sigs[ANA_REF_OPENLOOP]);
                }

                ccLogEnableAnaSignal(&ana_meas_sigs[ANA_B_MAGNET]);
                ccLogEnableAnaSignal(&ana_meas_sigs[ANA_B_MEAS]);
                ccLogEnableAnaSignal(&ana_meas_sigs[ANA_B_MEAS_FLTR]);
                ccLogEnableAnaSignal(&ana_meas_sigs[ANA_B_MEAS_EXTR]);
                ccLogEnableAnaSignal(&ana_meas_sigs[ANA_B_REF_DELAYED]);
                ccLogEnableAnaSignal(&ana_meas_sigs[ANA_B_ERR]);
                ccLogEnableAnaSignal(&ana_meas_sigs[ANA_MAX_ABS_B_ERR]);
                ccLogEnableDigSignal(&dig_meas_sigs[DIG_B_MEAS_TRIP]);
                ccLogEnableDigSignal(&dig_meas_sigs[DIG_B_REF_CLIP]);
                ccLogEnableDigSignal(&dig_meas_sigs[DIG_B_REF_RATE_CLIP]);


                if(ccpars_limits.b_low[ccpars_load.select] > 0.0)
                {
                    ccLogEnableDigSignal(&dig_meas_sigs[DIG_B_MEAS_LOW]);
                }

                if(ccpars_limits.b_zero[ccpars_load.select] > 0.0)
                {
                    ccLogEnableDigSignal(&dig_meas_sigs[DIG_B_MEAS_ZERO]);
                }

                if(ccpars_limits.b_err_warning[ccpars_load.select] > 0.0)
                {
                    ccLogEnableDigSignal(&dig_meas_sigs[DIG_B_REG_ERR_WARN]);
                }

                if(ccpars_limits.b_err_fault[ccpars_load.select] > 0.0)
                {
                    ccLogEnableDigSignal(&dig_meas_sigs[DIG_B_REG_ERR_FLT]);
                }
            }

            // Current regulation signals

            if(ccrun.is_ireg_enabled)
            {
                ccLogEnableAnaSignal(&ana_ireg_sigs[ANA_MEAS_REG]);
                ccLogEnableAnaSignal(&ana_ireg_sigs[ANA_REF]);
                ccLogEnableAnaSignal(&ana_ireg_sigs[ANA_REF_LIMITED]);
                ccLogEnableAnaSignal(&ana_ireg_sigs[ANA_REF_RST]);
                ccLogEnableAnaSignal(&ana_ireg_sigs[ANA_TRACK_DELAY]);

                if(ccpars_limits.i_neg[ccpars_load.select] >= 0.0)
                {
                    ccLogEnableAnaSignal(&ana_ireg_sigs[ANA_REF_OPENLOOP]);
                }

                ccLogEnableAnaSignal(&ana_meas_sigs[ANA_V_REF_SAT]);
                ccLogEnableAnaSignal(&ana_meas_sigs[ANA_I_REF_DELAYED]);
                ccLogEnableAnaSignal(&ana_meas_sigs[ANA_I_ERR]);
                ccLogEnableAnaSignal(&ana_meas_sigs[ANA_MAX_ABS_I_ERR]);
                ccLogEnableDigSignal(&dig_meas_sigs[DIG_I_REF_CLIP]);

                if(ccpars_limits.i_rate[ccpars_load.select] > 0.0)
                {
                    ccLogEnableDigSignal(&dig_meas_sigs[DIG_I_REF_RATE_CLIP]);
                }

                if(ccpars_limits.i_low[ccpars_load.select] > 0.0)
                {
                    ccLogEnableDigSignal(&dig_meas_sigs[DIG_I_MEAS_LOW]);
                }

                if(ccpars_limits.i_zero[ccpars_load.select] > 0.0)
                {
                    ccLogEnableDigSignal(&dig_meas_sigs[DIG_I_MEAS_ZERO]);
                }

                if(ccpars_limits.i_err_warning[ccpars_load.select] > 0.0)
                {
                    ccLogEnableDigSignal(&dig_meas_sigs[DIG_I_REG_ERR_WARN]);
                }

                if(ccpars_limits.i_err_fault[ccpars_load.select] > 0.0)
                {
                    ccLogEnableDigSignal(&dig_meas_sigs[DIG_I_REG_ERR_FLT]);
                }
            }

            // Current simulation signals

            if(reg_mgr.sim_load_pars.is_load_undersampled == false)
            {
                ccLogEnableAnaSignal(&ana_meas_sigs[ANA_I_MAGNET]);
            }
        }
        else // Converter actuation is CURRENT reference
        {
            ccLogEnableAnaSignal(&ana_meas_sigs[ANA_V_CIRCUIT]);
            ccLogEnableAnaSignal(&ana_meas_sigs[ANA_V_MEAS]);

            ccLogEnableAnaSignal(&ana_ireg_sigs[ANA_MEAS_REG]);
            ccLogEnableAnaSignal(&ana_ireg_sigs[ANA_REF]);
            ccLogEnableAnaSignal(&ana_ireg_sigs[ANA_REF_LIMITED]);
            ccLogEnableAnaSignal(&ana_meas_sigs[ANA_I_REF_DELAYED]);

            ccLogEnableDigSignal(&dig_meas_sigs[DIG_I_REF_CLIP]);

            if(ccpars_limits.i_rate[ccpars_load.select] > 0.0)
            {
                ccLogEnableDigSignal(&dig_meas_sigs[DIG_I_REF_RATE_CLIP]);
            }

            ana_ireg_sigs[ANA_MEAS_REG].time_offset = -reg_mgr.iter_period * (uint32_t)(reg_mgr.i.meas.delay_iters[ccpars_meas.i_reg_select] + 0.499);
        }

        // Current simulation signals

        ccLogEnableAnaSignal(&ana_meas_sigs[ANA_I_CIRCUIT]);
        ccLogEnableAnaSignal(&ana_meas_sigs[ANA_I_MEAS]);
        ccLogEnableAnaSignal(&ana_meas_sigs[ANA_I_MEAS_FLTR]);
        ccLogEnableAnaSignal(&ana_meas_sigs[ANA_I_MEAS_EXTR]);
        ccLogEnableDigSignal(&dig_meas_sigs[DIG_I_MEAS_TRIP]);

        if(ccpars_limits.i_low[ccpars_load.select] > 0.0)
        {
            ccLogEnableDigSignal(&dig_meas_sigs[DIG_I_MEAS_LOW]);
        }

        if(ccpars_limits.i_zero[ccpars_load.select] > 0.0)
        {
            ccLogEnableDigSignal(&dig_meas_sigs[DIG_I_MEAS_ZERO]);
        }

        // RMS current signals

        if(ccpars_limits.i_rms_tc > 0.0)
        {
            ccLogEnableAnaSignal(&ana_meas_sigs[ANA_I_RMS]);

            if(ccpars_limits.i_rms_warning > 0.0)
            {
                ccLogEnableDigSignal(&dig_meas_sigs[DIG_I_RMS_WARN]);
            }

            if(ccpars_limits.i_rms_fault > 0.0)
            {
                ccLogEnableDigSignal(&dig_meas_sigs[DIG_I_RMS_FLT]);
            }
        }

        // RMS_LOAD current signals

        if(ccpars_limits.i_rms_load_tc[ccpars_load.select] > 0.0)
        {
            ccLogEnableAnaSignal(&ana_meas_sigs[ANA_I_RMS_LOAD]);

            if(ccpars_limits.i_rms_load_warning[ccpars_load.select] > 0.0)
            {
                ccLogEnableDigSignal(&dig_meas_sigs[DIG_I_RMS_LOAD_WARN]);
            }

            if(ccpars_limits.i_rms_load_fault[ccpars_load.select] > 0.0)
            {
                ccLogEnableDigSignal(&dig_meas_sigs[DIG_I_RMS_LOAD_FLT]);
            }
        }

        // INVALID_MEAS signal

        if(ccrun.invalid_meas.random_threshold > 0)
        {
            ccLogEnableDigSignal(&dig_meas_sigs[DIG_INVALID_MEAS]);
        }
    }
}



void ccLogStoreReg(struct cclog *log, double iter_time)
{
    log->last_sample_time  = iter_time;
    log->last_sample_index = (log->last_sample_index + 1 ) % ccpars_global.log_length;

    if(log->num_samples < ccpars_global.log_length)
    {
        log->num_samples++;
    }

    ccLogStoreSignals(log);
}



void ccLogStoreMeas(double iter_time)
{
    meas_log.last_sample_time  = iter_time;
    meas_log.last_sample_index = (meas_log.last_sample_index + 1 ) % ccpars_global.log_length;

    if(meas_log.num_samples < ccpars_global.log_length)
    {
        meas_log.num_samples++;
    }

    // Take square room for RMS signals before they are logged

    i_rms      = sqrtf(reg_mgr.lim_i_rms.meas2_filter);
    i_rms_load = sqrtf(reg_mgr.lim_i_rms_load.meas2_filter);

    ccLogStoreSignals(&meas_log);
}



uint32_t ccLogReportBadValues(struct cclog *log)
{
    uint32_t    sig_idx;
    uint32_t    exit_status = EXIT_SUCCESS;

    // Scan all analogue signals and check the bad signal counters

    for(sig_idx = 0 ; sig_idx < log->num_ana_signals ; sig_idx++)
    {
        if(log->ana_sigs[sig_idx].is_enabled && log->ana_sigs[sig_idx].num_bad_values > 0)
        {
            printf("Bad values for %-20s : %6u\n",log->ana_sigs[sig_idx].name, log->ana_sigs[sig_idx].num_bad_values);
            exit_status = EXIT_FAILURE;
        }
    }

    return(exit_status);
}

// EOF
