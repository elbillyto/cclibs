/*---------------------------------------------------------------------------------------------------------*\
  File:     ccRun.c                                                                     Copyright CERN 2015

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

  Purpose:  This file contains the function that run the test as a function of time in one
            of three modes:

                - Simulation of voltage source and load with voltage, current or field regulation
                - Normal (increasing) time function generation tests
                - Reverse (descreasing) time function generation tests
\*---------------------------------------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// Include cctest program header files

#include "ccCmds.h"
#include "ccTest.h"
#include "ccFile.h"
#include "ccParse.h"
#include "ccRef.h"
#include "ccLog.h"
#include "ccRun.h"

/*---------------------------------------------------------------------------------------------------------*/
static uint32_t ccRunStartFunction(double iter_time, float *ref)
/*---------------------------------------------------------------------------------------------------------*\
  This function tests the ability to open and re-close the loop regulating current or field.  The user can
  specify the time and duration of a period of open-loop to be included with-in a run in closed loop.
\*---------------------------------------------------------------------------------------------------------*/
{
    float           delay;
    float           rate;

    // If a pre-function RAMP segment should be started

    if(ccrun.prefunc.idx < ccrun.prefunc.num_ramps)
    {
        float   prefunc_final_ref = 0.0;

        // If first pre-function segment

        if(ccrun.prefunc.idx == 0)
        {
            // Log max abs error for previous function

            switch(reg_mgr.reg_mode)
            {
            case REG_FIELD  : ccrun.cycle[ccrun.cycle_idx].max_abs_err = regMgrVar(reg_mgr, BREG_MAX_ABS_ERR); break;
            case REG_CURRENT: ccrun.cycle[ccrun.cycle_idx].max_abs_err = regMgrVar(reg_mgr, IREG_MAX_ABS_ERR); break;
            default:          ccrun.cycle[ccrun.cycle_idx].max_abs_err = 0.0;                                  break;
            }

            // If functions are finished then program a final plateau of duration GLOBAL STOP_DELAY

            if(++ccrun.cycle_idx >= ccrun.num_cycles)
            {
                delay = ccpars_global.stop_delay;
                rate  = 0.0;

                ccrun.prefunc.num_ramps = 1;
                prefunc_final_ref = *ref;
            }
            else // else start pre-function between two functions
            {
                float invert_limits = reg_mgr.reg_signal->lim_ref.invert_limits == REG_ENABLED ? -1.0 : 1.0;

                // Set cyc_sel for the next cycle

                ccrun.cyc_sel = ccrun.cycle[ccrun.cycle_idx].cyc_sel;

                // Set regulation mode for the next function

                regMgrModeSetRT(&reg_mgr, ccpars_ref[ccrun.cyc_sel].reg_mode);

                // Set up pre-function segment references according to the pre-function policy

                switch(ccpars_ref[ccrun.cyc_sel].prefunc_policy)
                {
                    case PREFUNC_RAMP:

                        ccrun.prefunc.num_ramps    = 1;
                        prefunc_final_ref          = ccrun.fg_pars[ccrun.cyc_sel].meta.range.initial_ref;
                        break;

                    case PREFUNC_MIN:

                        ccrun.prefunc.num_ramps    = 2;
                        prefunc_final_ref          = ccpars_ref[ccrun.cyc_sel].prefunc_min_ref * invert_limits;
                        ccrun.prefunc.final_ref[1] = ccrun.fg_pars[ccrun.cyc_sel].meta.range.initial_ref;
                        break;

                    case PREFUNC_MINMAX:

                        ccrun.prefunc.num_ramps    = 3;
                        prefunc_final_ref          = ccpars_ref[ccrun.cyc_sel].prefunc_min_ref * invert_limits;
                        ccrun.prefunc.final_ref[1] = reg_mgr.lim_ref->pos * invert_limits;
                        ccrun.prefunc.final_ref[2] = ccrun.fg_pars[ccrun.cyc_sel].meta.range.initial_ref;
                        break;
                }

                delay = 0;

                if(reg_mgr.reg_mode == REG_VOLTAGE)
                {
                    *ref = regRstPrevActRT (&reg_mgr.reg_signal->rst_vars);
                    rate = regRstAverageDeltaActRT(&reg_mgr.reg_signal->rst_vars) / reg_mgr.reg_period;
                }
                else
                {
                    *ref = regRstPrevRefRT (&reg_mgr.reg_signal->rst_vars);
                    rate = reg_mgr.reg_signal->rate.estimate;
                }

                if(prefunc_final_ref == *ref)
                {
                    delay = ccpars_default.plateau_duration;
                    rate  = 0.0;
                }
            }

            // Prepare to generate the first pre-function RAMP segment

            ccrun.fg_func      = fgRampRT;
            ccrun.fg_func_pars = (FG_pars *)&ccrun.prefunc.pars;
        }
        else // Prepare to generate the second or third pre-function RAMP segment
        {
            prefunc_final_ref = ccrun.prefunc.final_ref[ccrun.prefunc.idx];
            delay = ccpars_default.plateau_duration;
            rate  = 0.0;
        }

        // Arm a RAMP for the next pre-function segment - flip reference sign when limits are inverted

        fgRampCalc(ccpars_load.pol_swi_auto,
                   ccpars_limits.invert, 
                   delay,
                   rate,
                   *ref,
                   prefunc_final_ref,
                   ccpars_default.pars[reg_mgr.reg_mode].acceleration,
                   ccpars_default.pars[reg_mgr.reg_mode].linear_rate,
                   ccpars_default.pars[reg_mgr.reg_mode].deceleration,
                   &ccrun.prefunc.pars);

        ccrun.cycle_time_origin = iter_time + reg_mgr.ref_advance;

        if(reg_mgr.reg_mode != REG_VOLTAGE)
        {
            ccrun.cycle_time_origin -= reg_mgr.reg_signal->iteration_counter * reg_mgr.iter_period;
        }
        ccrun.prefunc.idx++;
    }
    else // Pre-function sequence complete - start the next function
    {
        enum FG_types func_idx;

        // If all functions completed

        if(ccrun.cycle_idx >= ccrun.num_cycles)
        {
            return(0);      // Return 0 to end simulation
        }

        // Set regulation mode (which won't change) to reset max abs error values

        regMgrModeSetRT(&reg_mgr, ccpars_ref[ccrun.cyc_sel].reg_mode);

        // Prepare to generate new function

        func_idx = ccpars_ref[ccrun.cyc_sel].function;

        ccrun.fg_func      = funcs[func_idx].fg_func;
        ccrun.fg_func_pars = &ccrun.fg_pars[ccrun.cyc_sel];

        ccrun.cycle[ccrun.cycle_idx].ref_advance = reg_mgr.ref_advance;
        ccrun.cycle[ccrun.cycle_idx].start_time  = iter_time + ccpars_global.run_delay;
        ccrun.cycle_time_origin = ccrun.cycle[ccrun.cycle_idx].start_time - ccrun.fg_pars[ccrun.cyc_sel].meta.time.start;
        ccrun.cycle_end_time    = ccrun.cycle_time_origin + ccrun.fg_pars[ccrun.cyc_sel].meta.time.end;

        // Reset pre-function index for when this new function ends

        ccrun.prefunc.idx       = 0;
        ccrun.prefunc.num_ramps = 1;

        // Reset dynamic economy

        ccrun.dyn_eco.fg_func     = NULL;
        ccrun.dyn_eco.time_offset = 0.0;

        // Reset table segment index in case function is DIRECT

        if(func_idx == FG_DIRECT)
        {
            ccrun.fg_pars[ccrun.cyc_sel].table.seg_idx = 0;
        }
    }

    return(1); // Return 1 to continue the simulation
}
/*---------------------------------------------------------------------------------------------------------*/
static bool ccDynamicEconomy(double iter_time, double ref_time, float ref)
/*---------------------------------------------------------------------------------------------------------*\
  This will manage the dynamic economy reference function that can take over a function for a specified
  period.
\*---------------------------------------------------------------------------------------------------------*/
{
    static float    final_ref;

    // Return immediately if running a pre-function, or a RAMP function, or time is before the start
    // of the dynamic economy window, or the end of the dyn_eco window is beyond the end of the function

    if(ccrun.prefunc.idx > 0                      ||
       ccrun.fg_func == fgRampRT                  ||
       ref_time < ccpars_global.dyn_eco_time[0]   ||
       ccpars_global.dyn_eco_time[1] > ccrun.cycle_end_time)
    {
        return(false);
    }

    // If dynamic economy PLEP needs to be initialised

    if(ccrun.dyn_eco.fg_func == NULL)
    {
        float  final_ref1;
        float  end_time  = ccpars_global.dyn_eco_time[1] - ccrun.cycle_time_origin + reg_mgr.ref_advance;;
        float  end_time1 = end_time + reg_mgr.reg_period;

        ccrun.dyn_eco.fg_func = ccrun.fg_func;

        // Evaluate function on consecutive regulation periods at the end of the window to work out the rate

        ccrun.fg_func(ccrun.fg_func_pars, end_time,  &final_ref);
        ccrun.fg_func(ccrun.fg_func_pars, end_time1, &final_ref1);

        // If the PLEP initialises without errors and the start time of the PLEP is still in the future

        if(fgPlepInit(  NULL, 
                        ccpars_load.pol_swi_auto,
                        ccpars_limits.invert, 
                        ref, 
                        final_ref,
                        (final_ref1 - final_ref) / reg_mgr.reg_period,
                        ccpars_default.pars[reg_mgr.reg_mode].acceleration,
                        ccpars_default.pars[reg_mgr.reg_mode].linear_rate,
                        0.0,
                        0.0,
                        (FG_pars *)&ccrun.dyn_eco.pars,
                        NULL) == FG_OK &&
           (ccrun.dyn_eco.time_offset = end_time - ccrun.dyn_eco.pars.meta.time.duration) >= ref_time)
        {
            uint32_t log_length = ccrun.dyn_eco.log.length++;

            // Log starting point

            ccrun.dyn_eco.log.time[log_length] = iter_time;
            ccrun.dyn_eco.log.ref [log_length] = ref;

            // Switch running function for PLEP to execute the dynamic economy section

            ccrun.dyn_eco.fg_func_pars = ccrun.fg_func_pars;

            ccrun.fg_func      = fgPlepRT;
            ccrun.fg_func_pars = (FG_pars *)&ccrun.dyn_eco.pars;

        }
        else
        {
            // Skip dynamic economy section completely

            ccrun.dyn_eco.fg_func_pars = NULL;
            ccrun.dyn_eco.time_offset  = 0.0;
        }
    }

    // If dynamic economy function was initialised successfully

    if(ccrun.dyn_eco.fg_func_pars != NULL)
    {
        if(iter_time >= ccpars_global.dyn_eco_time[1])
        {
            uint32_t log_length = ccrun.dyn_eco.log.length++;

            ccrun.dyn_eco.log.time[log_length] = ccpars_global.dyn_eco_time[1];
            ccrun.dyn_eco.log.ref [log_length] = final_ref;

            // Resume previous function

            ccrun.fg_func      = ccrun.dyn_eco.fg_func;
            ccrun.fg_func_pars = ccrun.dyn_eco.fg_func_pars;

            ccrun.dyn_eco.fg_func_pars = NULL;
            ccrun.dyn_eco.time_offset  = 0.0;

            return(false);
        }

        return(true);
    }

    return(false);
}
/*---------------------------------------------------------------------------------------------------------*/
static uint32_t ccRunAbort(double iter_time)
/*---------------------------------------------------------------------------------------------------------*\
  This will initialise a RAMP function that will take over the from the running function and will smoothly
  ramp to the minimum reference value given in the limits. This is only supported when regulating current
  or field.  In this example, the rate of change is calculated from the ref values in the RST history.
\*---------------------------------------------------------------------------------------------------------*/
{
    float  acceleration = reg_mgr.lim_ref->acceleration;

    // If acceleration limit is not set then base the acceleration limit on the rate limit

    if(acceleration <= 0.0)
    {
        acceleration = reg_mgr.lim_ref->rate / (10.0 * reg_mgr.reg_period);
    }

    // Initialise a RAMP to take over the running function.

    fgRampCalc(ccpars_load.pol_swi_auto,
               ccpars_limits.invert, 
               0.0,
               regRstDeltaRefRT(&reg_mgr.reg_signal->rst_vars) / reg_mgr.reg_period,  // last reference rate
               regRstPrevRefRT (&reg_mgr.reg_signal->rst_vars),                       // last reference value
               0.0,                                                             // final reference value
               acceleration,
               reg_mgr.lim_ref->rate,
               acceleration,
               &ccrun.prefunc.pars);

    // Check that abort duration is not too large (limit to 50000 iterations)
    // This can be a problem if the exponential decay is included and the exp_final parameter is too close
    // to zero.

    if(((ccrun.prefunc.pars.meta.time.duration - iter_time) / reg_mgr.iter_period) > 50000)
    {
        ccParsPrintError("aborting requires more than 50000 iterations : duration = %.1f\n",ccrun.prefunc.pars.meta.time.duration);
        return(EXIT_FAILURE);
    }

    ccrun.cycle_time_origin = iter_time + reg_mgr.ref_advance;

    ccrun.fg_func      =  fgRampRT;
    ccrun.fg_func_pars = (FG_pars*)&ccrun.prefunc.pars;

    // Cancel any subsequent functions

    if(ccrun.prefunc.idx == 0)
    {
        ccrun.num_cycles = ccrun.cycle_idx + 1;
    }
    else
    {
        ccrun.num_cycles = ccrun.cycle_idx;
    }
    ccrun.prefunc.idx  = 0;

    return(EXIT_SUCCESS);
}
/*---------------------------------------------------------------------------------------------------------*/
void ccRunSimulation(void)
/*---------------------------------------------------------------------------------------------------------*\
  This function will run a simulation of the voltage source and load. Regulation can be disabled (VOLTAGE)
  or enabled (FIELD or CURRENT). Multiple reference functions can be played in a sequence.
\*---------------------------------------------------------------------------------------------------------*/
{
    bool                is_abort_active        = false;    // Abort function flag
    float               ref;                               // Function generator reference value
    float               perturb_volts          = 0.0;      // Voltage perturbation to apply to circuit
    double              iter_time              = 0.0;      // Iteration time (since start of run)
    double              ref_time               = 0.0;      // Reference time (since start of function)
    double              trip_time              = 0.0;
    bool                is_max_abs_err_enabled = false;    // Control max_abs_err calculation
    enum FG_func_status fg_func_status;                    // Function generation status

    // Prepare to generation the first function with no pre-function

    ccrun.prefunc.idx       = 0;
    ccrun.prefunc.num_ramps = 0;
    ccrun.cycle_idx         = 0;
    ccrun.is_pc_tripped     = false;
    ccrun.cyc_sel           = ccrun.cycle[0].cyc_sel;

    // Call once with conv.reg_mode equal REG_NONE to set iteration counters

    regMgrMeasSetRT(&reg_mgr, ccrun.cycle[0].reg_rst_source, 0, 0, true, is_max_abs_err_enabled);

    ref = ccrun.fg_pars[ccrun.cyc_sel].meta.range.initial_ref;

    ccRunStartFunction(iter_time, &ref);

    // Loop until all functions have completed

    for(;;)
    {
        uint32_t        reg_iteration_counter;      // Regulation iteration counter from libreg (0=start of reg period)
        bool            use_sim_meas;
        int             rand_value = rand();        // Random int between 0 and RAND_MAX

        // Calculate reference time taking into account the ref advance for the active regulation mode

        ref_time = iter_time - ccrun.cycle_time_origin + reg_mgr.ref_advance;

        // Set measurements to simulated values but support bad value with a defined probability
        // The "real" measurements are invalid while the simulated measurements are good

        ccrun.invalid_meas.flag = (rand_value < ccrun.invalid_meas.random_threshold);

        use_sim_meas = (ccrun.invalid_meas.flag == 0);

        // Start new iteration by processing the measurements

        reg_iteration_counter = regMgrMeasSetRT(&reg_mgr, ccrun.cycle[ccrun.cycle_idx].reg_rst_source, 0, 0, 
                                                 use_sim_meas, is_max_abs_err_enabled);

        // If converter has not tripped

        if(ccrun.is_pc_tripped == false)
        {
            // If this is the first iteration of the regulation period then calculate a new reference value

            if(reg_iteration_counter == 0)
            {
                if(ccpars_global.dyn_eco_time[0] > 0.0 && ccDynamicEconomy(iter_time, ref_time, ref) == true)
                {
                    ref_time -= ccrun.dyn_eco.time_offset;
                }

                fg_func_status = ccrun.fg_func(ccrun.fg_func_pars, ref_time, &ref);
            }

            // Call ConvRegulate every iteration to keep RST histories up to date

            regMgrRegulateRT(&reg_mgr, &ref);

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

                else if(fg_func_status == FG_GEN_POST_FUNC && iter_time > ccrun.cycle_end_time)
                {
                    is_max_abs_err_enabled = false;

                    // Starting a new function can change conv.reg_mode, but not our local copy of reg_mode

                    if(ccRunStartFunction(iter_time, &ref) == 0)
                    {
                        break;
                    }
                }
                else
                {
                    is_max_abs_err_enabled = ccrun.prefunc.idx == 0 && fg_func_status != FG_GEN_PRE_FUNC;
                }
            }
        }

        // Apply voltage perturbation from the specified time

        if(iter_time >= ccpars_load.perturb_time && perturb_volts == 0.0)
        {
            perturb_volts = ccpars_load.perturb_volts;
        }

        // Simulate voltage source and load response (with voltage perturbation)

        regMgrSimulateRT(&reg_mgr, perturb_volts);

        // Check if simulated converter should be trip

        if(ccrun.is_pc_tripped == false)
        {
            if(reg_mgr.b.lim_meas.flags.trip      ||
               reg_mgr.i.lim_meas.flags.trip      ||
               reg_mgr.lim_i_rms.flags.fault      ||
               reg_mgr.lim_i_rms_load.flags.fault ||
               reg_mgr.b.err.fault.flag           ||
               reg_mgr.i.err.fault.flag           ||
               reg_mgr.v.err.fault.flag          )
            {
                // Simulate converter trip by switching to regulation mode to NONE

                trip_time = iter_time;
                ccrun.is_pc_tripped = true;

                regMgrModeSetRT(&reg_mgr, REG_NONE);

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

        if(ccrun.is_breg_enabled && reg_mgr.b.iteration_counter == 0)
        {
            ccLogStoreReg(&breg_log, iter_time);
        }

        // Store current regulation signals in log at regulation rate

        if(ccrun.is_ireg_enabled && reg_mgr.i.iteration_counter == 0)
        {
            ccLogStoreReg(&ireg_log, iter_time);
        }

        // Store measurement rate signals in log every iteration

        ccLogStoreMeas(iter_time);

        // Write all enabled log signals to CSV file (if CSW output is enabled)

        ccFileWriteCsvValues(iter_time);

        // Calculate next iteration time

        iter_time += reg_mgr.iter_period;
    }
}
/*---------------------------------------------------------------------------------------------------------*/
void ccRunFuncGen(void)
/*---------------------------------------------------------------------------------------------------------*\
  This function will test the function generator with one or more functions, with increasing time only.
\*---------------------------------------------------------------------------------------------------------*/
{
    uint32_t    cyc_sel;                    // Cycle selector for the current cycle
    uint32_t    func_idx;                   // Function index
    double      iter_time;                  // Iteration time (since start of run)
    double      ref_time;                   // Reference time (since start of function)
    enum FG_func_status fg_gen_status;       // Function generation status

    // Prepare to generation the first function

    cyc_sel                   = ccpars_global.cycle_selector[0];
    func_idx                  = ccpars_ref[cyc_sel].function;
    ccrun.cycle_idx           = 0;
    ccrun.cycle_time_origin   = 0.0;
    ccrun.cycle_end_time      = ccrun.fg_pars[cyc_sel].meta.time.end;
    ccrun.fg_func             = funcs[func_idx].fg_func;
    ccrun.fg_func_pars        = &ccrun.fg_pars[cyc_sel];
    iter_time                 = ccrun.fg_pars[cyc_sel].meta.time.start - ccpars_global.run_delay;
    ccrun.cycle[0].start_time = ccrun.fg_pars[cyc_sel].meta.time.start;

    // Loop until all functions have completed

    for(;;)
    {
        ref_time = iter_time - ccrun.cycle_time_origin;

        // Generate reference value using libfg function

        fg_gen_status = ccrun.fg_func(ccrun.fg_func_pars, ref_time, &reg_mgr.v.ref);

        // If reference function has finished

        if(ref_time > ccrun.cycle_end_time && fg_gen_status == FG_GEN_POST_FUNC)
        {
            // If not yet into the stop delay period, advance to next function

            if(ccrun.cycle_idx < ccrun.num_cycles)
            {
                // If another function is in the list

                if(++ccrun.cycle_idx < ccrun.num_cycles)
                {
                    // Prepare to generate the new function

                    cyc_sel              = ccpars_global.cycle_selector[ccrun.cycle_idx];
                    func_idx             = ccpars_ref[cyc_sel].function;
                    ccrun.cycle_end_time = ccrun.fg_pars[cyc_sel].meta.time.end;
                    ccrun.fg_func        = funcs[func_idx].fg_func;
                    ccrun.fg_func_pars   = &ccrun.fg_pars[cyc_sel];

                    ccrun.cycle_time_origin = iter_time + ccpars_default.plateau_duration - ccrun.fg_pars[cyc_sel].meta.time.start;

                    ccrun.cycle[ccrun.cycle_idx].start_time = ccrun.cycle_time_origin + ccrun.fg_pars[cyc_sel].meta.time.start;
                }
                else // else last function completed
                {
                    // Set duration to the stop delay and continue to use the last function

                    ccrun.cycle_end_time = ref_time + ccpars_global.stop_delay;
                }
            }
            else // else stop delay is complete so break out of loop
            {
                break;
            }
        }

        // Store and print to CSV file the enabled signals

        ccLogStoreMeas(iter_time);
        ccFileWriteCsvValues(iter_time);

        // Calculate next iteration time

        iter_time += reg_mgr.iter_period;
    }
}
/*---------------------------------------------------------------------------------------------------------*/
void ccRunFuncGenReverseTime(void)
/*---------------------------------------------------------------------------------------------------------*\
  This function will test a single function with decreasing sample time.
\*---------------------------------------------------------------------------------------------------------*/
{
    uint32_t    cyc_sel;                // Cycle selector for the current cycle
    uint32_t    iteration_idx;          // Iteration index
    double      func_time;              // Function time (runs backwards)
    double      end_time;               // End of function time

    // Reverse time can only be used with a single function

    cyc_sel              = ccpars_global.cycle_selector[0];
    ccrun.cycle_idx      = 0;
    ccrun.cycle_end_time = ccpars_global.run_delay + ccrun.fg_pars[cyc_sel].meta.time.duration + ccpars_global.stop_delay;
    ccrun.fg_func        = funcs[ccpars_ref[cyc_sel].function].fg_func;
    ccrun.fg_func_pars   = &ccrun.fg_pars[cyc_sel];
    end_time             = ccrun.fg_pars[cyc_sel].meta.time.end + ccpars_global.stop_delay;

    ccrun.num_iterations = (uint32_t)(1.4999 + ccrun.cycle_end_time / reg_mgr.iter_period);

    ccrun.cycle[0].start_time = 0.0;

    // Loop for the duration of the function plus stop delay

    for(iteration_idx = 0 ; iteration_idx <= ccrun.num_iterations ; iteration_idx++)
    {
        // Calculate iteration time

        func_time = end_time - reg_mgr.iter_period * iteration_idx;

        // Generate reference value using libfg function

        ccrun.fg_func(ccrun.fg_func_pars, func_time, &reg_mgr.v.ref);

        // Only support writing to CSV file

        ccFileWriteCsvValues(func_time);
    }
}
// EOF
