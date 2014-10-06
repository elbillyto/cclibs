/*---------------------------------------------------------------------------------------------------------*\
  File:     ccRun.c                                                                     Copyright CERN 2014

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

// Include cctest program header files

#include "ccCmds.h"
#include "ccTest.h"
#include "ccRef.h"
#include "ccSigs.h"
#include "ccRun.h"

/*---------------------------------------------------------------------------------------------------------*/
static uint32_t ccRunAbort(double iter_time)
/*---------------------------------------------------------------------------------------------------------*\
  This will initialize a RAMP function that will take over the from the running function and will smoothly
  ramp to the minimum reference value given in the limits. This is only supported when regulating current
  or field.  In this example, the rate of change is calculated from the ref values in the RST history.
\*---------------------------------------------------------------------------------------------------------*/
{
    struct fg_ramp_config  config;
    struct fg_meta         meta;

    // Set up RAMP configuration from limits (either current or field according to mode)

    config.final        = 0.0;
    config.linear_rate  = ccrun.fg_limits->rate;
    config.acceleration = ccrun.fg_limits->acceleration;

    // If acceleration limit is not set then base the acceleration limit on the rate limit

    if(config.acceleration <= 0.0)
    {
        config.acceleration = config.linear_rate / (10.0 * conv.reg_period);
    }

    // Make ramp symmetric with deceleration = acceleration

    config.deceleration = config.acceleration;

    // Initialize a RAMP to take over the running function.

    fgRampCalc(&config,
               iter_time,                                                       // time of last RST calculation
               regRstPrevRefRT(&conv.reg_signal->rst_vars),                     // last reference value
               regRstDeltaRefRT(&conv.reg_signal->rst_vars) / conv.reg_period,  // last reference rate
               &ccpars_ramp.pars,
               &meta);

    // Check that abort duration is not too large (limit to 50000 iterations)
    // This can be a problem if the exponential decay is included and the exp_final parameter is too close
    // to zero.

    if(((meta.duration - iter_time) / conv.iter_period) > 50000)
    {
        ccTestPrintError("aborting requires more than 50000 iterations : duration = %.1f\n",meta.duration);
        return(EXIT_FAILURE);
    }

    ccrun.fgen_func = funcs[FG_RAMP].fgen_func;
    ccrun.fg_pars   = funcs[FG_RAMP].fg_pars;

    return(EXIT_SUCCESS);
}
/*---------------------------------------------------------------------------------------------------------*/
static void ccRunStartFunction(uint32_t func_idx, double func_start_time)
/*---------------------------------------------------------------------------------------------------------*\
  This function tests the ability to open and re-close the loop regulating current or field.  The user can
  specify the time and duration of a period of open-loop to be included with-in a run in closed loop.
\*---------------------------------------------------------------------------------------------------------*/
{
    ccrun.func_duration = ccrun.fg_meta[func_idx].duration;
    ccrun.fgen_func     = funcs[ccpars_global.function[func_idx]].fgen_func;
    ccrun.fg_pars       = funcs[ccpars_global.function[func_idx]].fg_pars;

    regConvSetModeRT(&conv, ccpars_global.reg_mode[func_idx]);

    ccrun.ref_advance    [func_idx] = conv.ref_advance;
    ccrun.func_start_time[func_idx] = func_start_time;

    ccSigsStoreCursor(CSR_FUNC,function_type[func_idx].string);
}
/*---------------------------------------------------------------------------------------------------------*/
void ccRunSimulation(void)
/*---------------------------------------------------------------------------------------------------------*\
  This function will run a simulation of the voltage source and load. Regulation can be disabled (VOLTAGE)
  or enabled (FIELD or CURRENT). Multiple reference functions can be played in a sequence.
\*---------------------------------------------------------------------------------------------------------*/
{
    uint32_t    iteration_idx   = 0;        // Iteration index
    uint32_t    reg_iteration_counter;      // Regulation iteration counter
    uint32_t    func_run_f      = 1;        // Function running flag
    uint32_t    abort_f         = 0;        // Abort function flag
    uint32_t    good_meas_f;                // Good measurement flag
    float       ref;                        // Function generator reference value
    float       perturb_volts   = 0.0;      // Voltage perturbation to apply to circuit
    double      iter_time       = 0.0;      // Iteration time (since start of run)
    double      ref_time        = 0.0;      // Reference time (since start of function)
    double      func_start_time = 0.0;      // Function start time (relative to start of run)

    // Prepare to generation the first function

    ccrun.func_idx        = 0;
    ccrun.vs_tripped_flag = 0;

    // Call once with conv.reg_mode equal REG_NONE to set iteration counters

    regConvSetMeasRT(&conv, REG_OPERATIONAL_RST_PARS, 0, 0, 1);

    ccRunStartFunction(0,0);

    ref_time = conv.ref_advance;

    // Loop until all functions have completed

    for(;;)
    {
        // Set measurements to simulated values but support bad values at a given period
        // The "real" measurements are invalid while the simulated measurements are good

        good_meas_f = ccpars_meas.invalid_meas_period_iters == 0 ||
                     (iteration_idx % ccpars_meas.invalid_meas_period_iters) >= ccpars_meas.invalid_meas_repeat_iters;

        reg_iteration_counter = regConvSetMeasRT(&conv, REG_OPERATIONAL_RST_PARS, 0, 0, good_meas_f);

        // If converter has not tripped

        if(ccrun.vs_tripped_flag == 0)
        {
            // if first iteration of regulation period then calculate new reference value

            if(reg_iteration_counter == 0)
            {
                func_run_f = ccrun.fgen_func(ccrun.fg_pars, &ref_time, &ref);

                regConvRegulateRT(&conv, &ref);

               // Check for function abort based on the abort time

                if(abort_f == 0 && ccpars_global.abort_time > 0.0 && iter_time >= ccpars_global.abort_time)
                {
                    // If attempt to abort fails then stop simulation

                    if(ccRunAbort(ref_time) == EXIT_FAILURE)
                    {
                        break;
                    }

                    abort_f = 1;
                }
            }
        }

        // Apply voltage perturbation from the specified time

        if(iter_time >= ccpars_load.perturb_time && perturb_volts == 0.0)
        {
            perturb_volts = ccpars_load.perturb_volts;
        }

        // Simulate voltage source and load response (with voltage perturbation added)

        regConvSimulateRT(&conv, perturb_volts);

        // Store and print to CSV file the enabled signals

        ccSigsStore(iter_time);

        // Check if any condition requires the converter to trip

        if(ccrun.vs_tripped_flag           == 0 &&
          (conv.b.lim_meas.flags.trip      == 1 ||
           conv.i.lim_meas.flags.trip      == 1 ||
           conv.lim_i_rms.flags.fault      == 1 ||
           conv.lim_i_rms_load.flags.fault == 1 ||
           conv.b.err.fault.flag           == 1 ||
           conv.i.err.fault.flag           == 1 ||
           conv.v.err.fault.flag           == 1))
        {
            // Simulate converter trip - switch regulation mode to NONE

            ccrun.vs_tripped_flag = 1;

            regConvSetModeRT(&conv, REG_NONE);

            ccSigsStoreCursor(CSR_FUNC,"TRIP!");

            puts("Trip");

            // Run for the stop delay following the trip

            func_run_f          = 0;
            ccrun.func_idx      = ccrun.num_functions;
            ccrun.func_duration = ref_time + ccpars_global.stop_delay;
        }

        // Calculate next iteration time

        iter_time = conv.iter_period * ++iteration_idx;

        ref_time = iter_time - func_start_time + conv.ref_advance;

        // If reference function has finished

        if(ref_time >= ccrun.func_duration && func_run_f == 0)
        {
            // If not yet into the stop delay period, advance to next function

            if(ccrun.func_idx < ccrun.num_functions)
            {
                // Record max absolute regulation error for debugging output

                switch(conv.reg_mode)
                {
                case REG_FIELD  : ccrun.max_abs_err[ccrun.func_idx] = conv.b.err.max_abs_err; break;
                case REG_CURRENT: ccrun.max_abs_err[ccrun.func_idx] = conv.i.err.max_abs_err; break;
                default:          ccrun.max_abs_err[ccrun.func_idx] = 0.0;                    break;
                }

                // If another function is in the list

                if(++ccrun.func_idx < ccrun.num_functions)
                {
                    // Prepare to generate the new function

                    func_start_time += ccrun.func_duration + ccpars_global.pre_func_delay;

                    ccRunStartFunction(ccrun.func_idx, func_start_time);

                    ref_time = iter_time - func_start_time + conv.ref_advance;
                }
                else // else last function completed
                {
                    // Set duration to the stop delay and continue to use the last function

                    ccrun.func_duration = ref_time + ccpars_global.stop_delay;
                }
            }
            else // else stop delay is complete so break out of loop
            {
                break;
            }
        }
    }
}
/*---------------------------------------------------------------------------------------------------------*/
void ccRunFuncGen(void)
/*---------------------------------------------------------------------------------------------------------*\
  This function will test the function generator with one or more functions, with increasing time only.
\*---------------------------------------------------------------------------------------------------------*/
{
    uint32_t    iteration_idx   = 0;        // Iteration index
    uint32_t    func_run_f      = 1;        // Function running flag
    double      iter_time       = 0.0;      // Iteration time (since start of run)
    double      ref_time        = 0.0;      // Reference time (since start of function)
    double      func_start_time = 0.0;      // Function start time (relative to start of run)

    // Prepare to generation the first function

    ccrun.func_idx      = 0;
    ccrun.func_duration = ccrun.fg_meta[0].duration;
    ccrun.fgen_func     = funcs[ccpars_global.function[0]].fgen_func;
    ccrun.fg_pars       = funcs[ccpars_global.function[0]].fg_pars;

    // Loop until all functions have completed

    for(;;)
    {
        // Generate reference value using libfg function

        func_run_f = ccrun.fgen_func(ccrun.fg_pars, &ref_time, &conv.v.ref);

        // Store and print to CSV file the enabled signals

        ccSigsStore(iter_time);

        // Calculate next iteration time

        iter_time = conv.iter_period * ++iteration_idx;

        ref_time = iter_time - func_start_time;

        // If reference function has finished

        if(ref_time >= ccrun.func_duration && func_run_f == 0)
        {
            // If not yet into the stop delay period, advance to next function

            if(ccrun.func_idx < ccrun.num_functions)
            {
                // If another function is in the list

                if(++ccrun.func_idx < ccrun.num_functions)
                {
                    // Prepare to generate the new function

                    func_start_time    += ccrun.func_duration + ccpars_global.pre_func_delay;
                    ccrun.func_duration = ccrun.fg_meta[ccrun.func_idx].duration;
                    ccrun.fgen_func     = funcs[ccpars_global.function[ccrun.func_idx]].fgen_func;
                    ccrun.fg_pars       = funcs[ccpars_global.function[ccrun.func_idx]].fg_pars;
                    ref_time            = iter_time - func_start_time;
                }
                else // else last function completed
                {
                    // Set duration to the stop delay and continue to use the last function

                    ccrun.func_duration += ccpars_global.stop_delay;
                }
            }
            else // else stop delay is complete so break out of loop
            {
                break;
            }
        }
    }
}
/*---------------------------------------------------------------------------------------------------------*/
void ccRunFuncGenReverseTime(void)
/*---------------------------------------------------------------------------------------------------------*\
  This function will test a single function with decreasing sample time.
\*---------------------------------------------------------------------------------------------------------*/
{
    uint32_t    iteration_idx;          // Iteration index
    double      iter_time;              // Iteration time (runs backwards)

    // Reverse time can only be used with a single function

    ccrun.func_duration = ccrun.fg_meta[0].duration + ccpars_global.stop_delay;

    ccrun.num_iterations = (uint32_t)(1.4999 + ccrun.func_duration / conv.iter_period);

    ccrun.fgen_func = funcs[ccpars_global.function[0]].fgen_func;
    ccrun.fg_pars   = funcs[ccpars_global.function[0]].fg_pars;

    // Loop for the duration of the function plus stop delay

    for(iteration_idx = 0 ; iteration_idx < ccrun.num_iterations ; iteration_idx++)
    {
        // Calculate iteration time

        iter_time = conv.iter_period * (ccrun.num_iterations - iteration_idx - 1);

        // Generate reference value using libfg function

        ccrun.fgen_func(ccrun.fg_pars, &iter_time, &conv.v.ref);

        // Store and print enabled signals

        ccSigsStore(iter_time);
    }
}
// EOF
