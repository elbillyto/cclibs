/*!
 * @file  ccrt/scr/ccFlot.c
 *
 * @brief ccrt Flot chart creation functions
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

#include "ccCmds.h"
#include "ccRt.h"
#include "ccParse.h"
#include "ccRun.h"
#include "ccLog.h"
#include "ccFlot.h"
#include "flot.h"



static void ccFlotAnalog(FILE *f, struct cclog *log, double time_origin, uint32_t period_iters)
{
    uint32_t       sig_idx;
    double         period       = (double)period_iters * conv.iter_period;
    uint32_t       num_periods  = (uint32_t)(ccpars_global.log_duration / period) - 1;

    // Write each signal in the log

    for(sig_idx = 0 ; sig_idx < log->num_ana_signals ; sig_idx++)
    {
        struct cclog_ana_sigs *ana_sig = &log->ana_sigs[sig_idx];
        double    first_sample_time;
        uint32_t  buf_idx;
        uint32_t  iteration_idx;
        float    *buf = ana_sig->buf;
        float     last_value;

        // Calculate index of first sample

        buf_idx = (log->last_sample_index - num_periods + CC_LOG_LENGTH) % CC_LOG_LENGTH;

        first_sample_time = log->last_sample_time - time_origin - (double)num_periods * period;

        fprintf(f,"\"%s\": { lines: { steps:%s }, points: { show:false }, %s\ndata:[",
                ana_sig->name,
                ana_sig->is_trailing_step ? "true" : "false",
                ana_sig->is_trailing_step ? "downsample: { threshold: 0 }," : "");

        for(iteration_idx = 0; iteration_idx <= num_periods; iteration_idx++)
        {
            // Only print changed values when meta_data is TRAIL_STEP

            if(iteration_idx == 0                 ||
               iteration_idx == num_periods       ||
               ana_sig->is_trailing_step == false ||
               buf[buf_idx] != last_value)
            {
                fprintf(f,"[%.6f,%.7E],", first_sample_time + period * (double)iteration_idx, buf[buf_idx]);
            }

            last_value = buf[buf_idx];
            buf_idx = (buf_idx + 1) % CC_LOG_LENGTH;
        }

        fputs("]\n },\n",f);
    }
}



void ccFlot(FILE *f, char *converter_name, double time_origin)
{
    // Print start of FLOT html page including flot path to all the javascript libraries

    fprintf(f,flot[0],converter_name,FLOT_PATH,FLOT_PATH,FLOT_PATH,FLOT_PATH,FLOT_PATH,FLOT_PATH,FLOT_PATH,FLOT_PATH);

    // Print enabled analog signal values

    ccFlotAnalog(f, &breg_log, time_origin, conv.b.reg_period_iters);
    ccFlotAnalog(f, &ireg_log, time_origin, conv.i.reg_period_iters);
    ccFlotAnalog(f, &meas_log, time_origin, 1);

    // Print the rest of the web page

    fprintf(f, flot[1], CC_VERSION);    // Version is embedded in the About pop-up title: "About ccrt vx.xx"
}

// EOF
