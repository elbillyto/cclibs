/*!
 * @file  ccrt/scr/ccLog.c
 *
 * @brief ccrt logging functions
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
#include <string.h>

#include "ccCmds.h"
#include "ccRt.h"
#include "ccFile.h"
#include "ccRun.h"
#include "ccLog.h"
#include "ccFlot.h"



static void ccLogStoreSignals(struct cclog *log)
{
    uint32_t sig_idx;
    float    value;

    // Store analogue signals

    for(sig_idx = 0 ; sig_idx < log->num_ana_signals ; sig_idx++)
    {
        struct cclog_ana_sigs *ana_sig = &log->ana_sigs[sig_idx];

        value = *ana_sig->source;

        // Protect against bad values and count how many occur

        if(!isfinite(value) || fabs(value) > 1.0E6)
        {
            value = 0.0;
            ana_sig->num_bad_values++;
        }

        ana_sig->buf[log->last_sample_index] = value;
    }
}



void ccLogStoreReg(struct cclog *log, double iter_time)
{
    log->last_sample_time  = iter_time;
    log->last_sample_index = (log->last_sample_index + 1 ) % CC_LOG_LENGTH;

    ccLogStoreSignals(log);
}



void ccLogStoreMeas(double iter_time)
{
    meas_log.last_sample_time  = iter_time;
    meas_log.last_sample_index = (meas_log.last_sample_index + 1 ) % CC_LOG_LENGTH;

    // Take square room for RMS signals before they are logged

    i_rms      = sqrtf(conv.lim_i_rms.meas2_filter);
    i_rms_load = sqrtf(conv.lim_i_rms_load.meas2_filter);

    ccLogStoreSignals(&meas_log);
}



uint32_t ccLogReportBadValues(struct cclog *log)
{
    uint32_t    sig_idx;
    uint32_t    exit_status = EXIT_SUCCESS;

    // Scan all analogue signals and check the bad signal counters

    for(sig_idx = 0 ; sig_idx < log->num_ana_signals ; sig_idx++)
    {
        if(log->ana_sigs[sig_idx].num_bad_values > 0)
        {
            printf("Bad values for %-20s : %6u\n",log->ana_sigs[sig_idx].name, log->ana_sigs[sig_idx].num_bad_values);
            log->ana_sigs[sig_idx].num_bad_values = 0;
            exit_status = EXIT_FAILURE;
        }
    }

    return(exit_status);
}

// EOF
