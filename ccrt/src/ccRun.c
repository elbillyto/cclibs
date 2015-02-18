/*!
 * @file  ccrt/scr/ccRun.c
 *
 * @brief ccrt real-time processing functions
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
#include <math.h>

// Include cctest program header files

#include "ccCmds.h"
#include "ccRt.h"
#include "ccFile.h"
#include "ccParse.h"
#include "ccRef.h"
#include "ccLog.h"
#include "ccRun.h"



static uint32_t ccRunStartFunction(double iter_time, float *ref)
{
    float           delay;
    float           rate;
    struct fg_meta  meta;
    uint32_t        num_cycles = global_pars[GLOBAL_CYCLE_SELECTOR].num_elements[0];

    // If a pre-function RAMP segment should be started

    if(ccrun.prefunc.idx < ccrun.prefunc.num_ramps)
    {
        float   prefunc_final_ref = 0.0;

        // If first pre-function segment

        if(ccrun.prefunc.idx == 0)
        {
            // If functions are finished then program a final plateau of duration GLOBAL STOP_DELAY

            if(++ccrun.cycle_idx >= num_cycles)
            {
                ccrun.cycle_idx = 0;
            }

            // start pre-function between two functions

            {
                float invert_limits = conv.reg_signal->lim_ref.invert_limits == REG_ENABLED ? -1.0 : 1.0;

                // Set cyc_sel for the next cycle

                ccrun.cyc_sel = ccrun.cycle[ccrun.cycle_idx].cyc_sel;

                // Set regulation mode for the next function

                regConvModeSetRT(&conv, ccpars_ref[ccrun.cyc_sel].reg_mode);

                // Set up pre-function segment references according to the pre-function policy

                switch(ccpars_prefunc.policy)
                {
                    case PREFUNC_RAMP:

                        ccrun.prefunc.num_ramps    = 1;
                        prefunc_final_ref          = ccpars_ref[ccrun.cyc_sel].fg_meta.range.start;
                        break;

                    case PREFUNC_UPMINMAX:
                    case PREFUNC_DOWNMAXMIN:

                        ccrun.prefunc.num_ramps    = 3;
                        prefunc_final_ref          = ccpars_prefunc.min * invert_limits;
                        ccrun.prefunc.final_ref[1] = ccpars_prefunc.max * invert_limits;
                        ccrun.prefunc.final_ref[2] = ccpars_ref[ccrun.cyc_sel].fg_meta.range.start;
                        break;
                }

                delay = 0;

                if(conv.reg_mode == REG_VOLTAGE)
                {
                    *ref = regRstPrevActRT (&conv.reg_signal->rst_vars);
                    rate = regRstAverageDeltaActRT(&conv.reg_signal->rst_vars) / conv.reg_period;
                }
                else
                {
                    *ref = regRstPrevRefRT (&conv.reg_signal->rst_vars);
                    rate = conv.reg_signal->rate.estimate;
                }

                if(prefunc_final_ref == *ref)
                {
                    delay = ccpars_prefunc.plateau_duration;
                    rate  = 0.0;
                }

            }

            // Prepare to generate the first pre-function RAMP segment

            ccrun.fgen_func = fgRampGen;
            ccrun.fgen_pars = &ccrun.prefunc.pars;
        }
        else // Prepare to generate the second or third pre-function RAMP segment
        {
            prefunc_final_ref = ccrun.prefunc.final_ref[ccrun.prefunc.idx];
            delay = ccpars_prefunc.plateau_duration;
            rate  = 0.0;
        }

        // Arm a RAMP for the next pre-function segment - flip reference sign when limits are inverted

        fgRampCalc(ccrun.polswitch.automatic,
                   ccrun.polswitch.negative,
                   delay,
                   rate,
                   *ref,
                   prefunc_final_ref,
                   ccpars_default.pars[conv.reg_mode].acceleration,
                   ccpars_default.pars[conv.reg_mode].linear_rate,
                   ccpars_default.pars[conv.reg_mode].deceleration,
                   &ccrun.prefunc.pars,
                   &meta);

        ccrun.cycle_start_time = iter_time + conv.ref_advance;

        if(conv.reg_mode != REG_VOLTAGE)
        {
            ccrun.cycle_start_time -= conv.reg_signal->iteration_counter * conv.iter_period;
        }
        ccrun.prefunc.idx++;
    }
    else // Pre-function sequence complete - start the next function
    {
        uint32_t func_idx;

        // If all functions completed

        if(ccrun.cycle_idx >= num_cycles)
        {
            return(0);      // Return 0 to end simulation
        }

        // Set regulation mode (which won't change) to reset max abs error values

        regConvModeSetRT(&conv, ccpars_ref[ccrun.cyc_sel].reg_mode);

        // Prepare to generate new function

        func_idx = ccpars_ref[ccrun.cyc_sel].function;

        ccrun.fgen_func = funcs[func_idx].fgen_func;
        ccrun.fgen_pars = funcs[func_idx].fg_pars + funcs[func_idx].size_of_pars * ccrun.cyc_sel;

        ccrun.cycle_duration = ccpars_global.run_delay + ccpars_ref[ccrun.cyc_sel].fg_meta.duration;

        ccrun.cycle_start_time = iter_time;

        ccrun.cycle[ccrun.cycle_idx].ref_advance = conv.ref_advance;
        ccrun.cycle[ccrun.cycle_idx].start_time  = iter_time;

        // Reset pre-function index for when this new function ends

        ccrun.prefunc.idx       = 0;
        ccrun.prefunc.num_ramps = 1;
    }

    return(1); // Return 1 to continue the simulation
}


/*
void ccRunSimulation(void)
{
    uint32_t           iteration_idx          = 0;        // Iteration index
    bool               is_abort_active        = false;    // Abort function flag
    float              ref;                               // Function generator reference value
    float              perturb_volts          = 0.0;      // Voltage perturbation to apply to circuit
    double             iter_time              = 0.0;      // Iteration time (since start of run)
    double             ref_time               = 0.0;      // Reference time (since start of function)
    double             trip_time              = 0.0;
    bool               is_max_abs_err_enabled = false;    // Control max_abs_err calculation
    enum fg_gen_status fg_gen_status;                     // Function generation status

    // Prepare to generation the first function with no pre-function

    ccrun.prefunc.idx       = 0;
    ccrun.prefunc.num_ramps = 0;
    ccrun.cycle_idx         = 0;
    ccrun.is_pc_tripped     = false;
    ccrun.cyc_sel           = ccrun.cycle[0].cyc_sel;

    // Call once with conv.reg_mode equal REG_NONE to set iteration counters

    regConvMeasSetRT(&conv, ccrun.cycle[0].reg_rst_source, 0, 0, true, is_max_abs_err_enabled);

    ref = ccrun.fg_meta[ccrun.cyc_sel].range.start;

    ccRunStartFunction(iter_time, &ref);

    // Loop until all functions have completed

    for(;;)
    {
        uint32_t        reg_iteration_counter;      // Regulation iteration counter from libreg (0=start of reg period)
        bool            use_sim_meas;
        int             rand_value = rand();        // Random int between 0 and RAND_MAX

        // Calculate reference time taking into account the ref advance for the active regulation mode

        ref_time = iter_time - ccrun.cycle_start_time + conv.ref_advance;

        // Set measurements to simulated values but support bad value with a defined probability
        // The "real" measurements are invalid while the simulated measurements are good

        ccrun.invalid_meas.flag = (rand_value < ccrun.invalid_meas.random_threshold);

        use_sim_meas = (ccrun.invalid_meas.flag == 0);

        // Start new iteration by processing the measurements

        reg_iteration_counter = regConvMeasSetRT(&conv, ccrun.cycle[ccrun.cycle_idx].reg_rst_source, 0, 0,
                                                 use_sim_meas, is_max_abs_err_enabled);

        // If converter has not tripped

        if(ccrun.is_pc_tripped == false)
        {
            // If this is the first iteration of the regulation period then calculate a new reference value

            if(reg_iteration_counter == 0)
            {
                if(ccpars_global.dyn_eco_time[0] > 0.0)
                {
                    ccDynamicEconomy(ref_time, ref);
                }

                fg_gen_status = ccrun.fgen_func(ccrun.fgen_pars, &ref_time, &ref);

                is_max_abs_err_enabled = ccrun.prefunc.idx == 0 && fg_gen_status == FG_GEN_DURING_FUNC;
            }

            // Call ConvRegulate every iteration to keep RST histories up to date

            regConvRegulateRT(&conv, &ref);

            // If this is the first iteration of the regulation period then check for function abort or function end

            if(reg_iteration_counter == 0)
            {
                // If function should be aborted

                if(is_abort_active == false && ccpars_global.abort_time > 0.0 && iter_time >= ccpars_global.abort_time)
                {
                    is_abort_active = true;

                    if(ccRunAbort(iter_time) == EXIT_FAILURE)
                    {
                        break;
                    }
                }

                // else if function has finished

                else if(fg_gen_status == FG_GEN_POST_FUNC)
                {
                    // Starting a new function can change conv.reg_mode, but not our local copy of reg_mode

                    if(ccRunStartFunction(iter_time, &ref) == 0)
                    {
                        break;
                    }
                }
            }

            // Apply voltage reference quantisation if specified

            if(ccpars_pc.quantization > 0.0)
            {
                conv.v.ref_limited = ccpars_pc.quantization * nearbyintf(conv.v.ref_limited / ccpars_pc.quantization);
            }
        }

        // Apply voltage perturbation from the specified time

        if(iter_time >= ccpars_load.perturb_time && perturb_volts == 0.0)
        {
            perturb_volts = ccpars_load.perturb_volts;
        }

        // Simulate voltage source and load response (with voltage perturbation and ripple added)

        regConvSimulateRT(&conv, NULL, perturb_volts + ccpars_pc.ripple * (float)(rand_value - RAND_MAX/2) * (1.0 / RAND_MAX));

        // Check if simulated converter should be trip

        if(ccrun.is_pc_tripped == false)
        {
            if(conv.b.lim_meas.flags.trip      ||
               conv.i.lim_meas.flags.trip      ||
               conv.lim_i_rms.flags.fault      ||
               conv.lim_i_rms_load.flags.fault ||
               conv.b.err.fault.flag           ||
               conv.i.err.fault.flag           ||
               conv.v.err.fault.flag          )
            {
                // Simulate converter trip by switching to regulation mode to NONE

                trip_time = iter_time;
                ccrun.is_pc_tripped = true;

                regConvModeSetRT(&conv, REG_NONE);

                puts("Trip");

                // Cancel any subsequent functions

                if(ccrun.prefunc.idx == 0)
                {
                    ccrun.num_cycles = ccrun.cycle_idx + 1;
                }
                else
                {
                    ccrun.num_cycles = ccrun.cycle_idx;
                }
            }
        }
        else if((iter_time - trip_time) > ccpars_global.stop_delay)
        {
            // After a trip, stop after STOP_DELAY

            break;
        }

        // Store field regulation signals in log at regulation rate

        if(ccrun.is_breg_enabled && conv.b.iteration_counter == 0)
        {
            ccLogStoreReg(&breg_log, iter_time);
        }

        // Store current regulation signals in log at regulation rate

        if(ccrun.is_ireg_enabled && conv.i.iteration_counter == 0)
        {
            ccLogStoreReg(&ireg_log, iter_time);
        }

        // Store measurement rate signals in log every iteration

        ccLogStoreMeas(iter_time);

        // Calculate next iteration time

        iter_time = conv.iter_period * ++iteration_idx;
    }
}
*/
// EOF
