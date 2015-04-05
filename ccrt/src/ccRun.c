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
#include <time.h>
#include <signal.h>
#include <math.h>

// Include cctest program header files

#include "ccCmds.h"
#include "ccRt.h"
#include "ccFile.h"
#include "ccParse.h"
#include "ccRef.h"
#include "ccLog.h"
#include "ccRun.h"
#include "ccSim.h"



static bool ccRunFaultLatch(enum REG_enabled_disabled *fault, bool flag)
{
    if(ccpars_pc.state == PC_ON)
    {
        *fault = *fault == REG_ENABLED || flag ? REG_ENABLED : REG_DISABLED;
    }
    else
    {
        *fault = flag ? REG_ENABLED : REG_DISABLED;
    }

    return(*fault == REG_ENABLED);
}



static void ccRunWarning(enum REG_enabled_disabled *warning, bool flag)
{
    *warning = flag ? REG_ENABLED : REG_DISABLED;
}



static void ccRunFaultsAndWarnings(void)
{
    bool    sum_of_faults = false;

    // Latch active faults and accumulate the sum of faults

    sum_of_faults |= ccRunFaultLatch(&ccpars_faults.b_meas_invalid,
                        regMgrVar(REG_mgr, MEAS_B_INVALID_SEQ_COUNTER) > regMgrVar(REG_mgr, BREG_PERIOD_ITERS));
    sum_of_faults |= ccRunFaultLatch(&ccpars_faults.i_meas_invalid,
                        regMgrVar(REG_mgr, MEAS_I_INVALID_SEQ_COUNTER) > regMgrVar(REG_mgr, IREG_PERIOD_ITERS));

    sum_of_faults |= ccRunFaultLatch(&ccpars_faults.b_meas_limit,   regMgrVar(REG_mgr, FLAG_B_MEAS_TRIP)       );
    sum_of_faults |= ccRunFaultLatch(&ccpars_faults.b_reg_err,      regMgrVar(REG_mgr, FLAG_B_REG_ERR_FAULT)   );
    sum_of_faults |= ccRunFaultLatch(&ccpars_faults.i_meas_limit,   regMgrVar(REG_mgr, FLAG_I_MEAS_TRIP)       );
    sum_of_faults |= ccRunFaultLatch(&ccpars_faults.i_reg_err,      regMgrVar(REG_mgr, FLAG_I_REG_ERR_FAULT)   );
    sum_of_faults |= ccRunFaultLatch(&ccpars_faults.v_reg_err,      regMgrVar(REG_mgr, FLAG_V_REG_ERR_FAULT)   );
    sum_of_faults |= ccRunFaultLatch(&ccpars_faults.i_rms,          regMgrVar(REG_mgr, FLAG_I_RMS_FAULT)       );
    sum_of_faults |= ccRunFaultLatch(&ccpars_faults.i_rms_load,     regMgrVar(REG_mgr, FLAG_I_RMS_LOAD_FAULT)  );
    sum_of_faults |= ccRunFaultLatch(&ccpars_faults.polswitch,      ccpars_state.polswitch == POLSWITCH_FAULT);

    ccrun.fault = sum_of_faults;

    // Set warnings parameters

    ccRunWarning(&ccpars_warnings.b_reg_err,  regMgrVar(REG_mgr, FLAG_B_REG_ERR_WARNING) );
    ccRunWarning(&ccpars_warnings.i_reg_err,  regMgrVar(REG_mgr, FLAG_I_REG_ERR_WARNING) );
    ccRunWarning(&ccpars_warnings.v_reg_err,  regMgrVar(REG_mgr, FLAG_V_REG_ERR_WARNING) );
    ccRunWarning(&ccpars_warnings.i_rms,      regMgrVar(REG_mgr, FLAG_I_RMS_WARNING)     );
    ccRunWarning(&ccpars_warnings.i_rms_load, regMgrVar(REG_mgr, FLAG_I_RMS_LOAD_WARNING));
}



static uint32_t ccRunStartFunction(double iter_time, float *ref)
{
    float           delay;
    float           rate;
    struct FG_meta  meta;
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
                float invert_limits = reg_mgr.reg_signal->lim_ref.invert_limits == REG_ENABLED ? -1.0 : 1.0;

                // Set cyc_sel for the next cycle

                ccrun.cyc_sel = ccrun.cycle[ccrun.cycle_idx].cyc_sel;

                // Set regulation mode for the next function

                regMgrModeSetRT(&reg_mgr, ccpars_ref[ccrun.cyc_sel].reg_mode);

                // Set up pre-function segment references according to the pre-function policy

                switch(ccpars_prefunc.policy)
                {
                    case PREFUNC_RAMP:

                        ccrun.prefunc.num_ramps    = 1;
                        prefunc_final_ref          = ccpars_ref[ccrun.cyc_sel].fg_meta.range.initial_ref;
                        break;

                    case PREFUNC_UPMINMAX:
                    case PREFUNC_DOWNMAXMIN:

                        ccrun.prefunc.num_ramps    = 3;
                        prefunc_final_ref          = ccpars_prefunc.min * invert_limits;
                        ccrun.prefunc.final_ref[1] = ccpars_prefunc.max * invert_limits;
                        ccrun.prefunc.final_ref[2] = ccpars_ref[ccrun.cyc_sel].fg_meta.range.initial_ref;
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
                    delay = ccpars_prefunc.plateau_duration;
                    rate  = 0.0;
                }

            }

            // Prepare to generate the first pre-function RAMP segment

            ccrun.fgen_func = fgRampRT;
            ccrun.fg_func_pars = &ccrun.prefunc.pars;
        }
        else // Prepare to generate the second or third pre-function RAMP segment
        {
            prefunc_final_ref = ccrun.prefunc.final_ref[ccrun.prefunc.idx];
            delay = ccpars_prefunc.plateau_duration;
            rate  = 0.0;
        }

        // Arm a RAMP for the next pre-function segment - flip reference sign when limits are inverted

        fgRampCalc(ccsim.polswitch.automatic,
                   ccsim.polswitch.negative,
                   delay,
                   rate,
                   *ref,
                   prefunc_final_ref,
                   ccpars_default.pars[reg_mgr.reg_mode].acceleration,
                   ccpars_default.pars[reg_mgr.reg_mode].linear_rate,
                   ccpars_default.pars[reg_mgr.reg_mode].deceleration,
                   &ccrun.prefunc.pars,
                   &meta);

        ccrun.cycle_time_origin = iter_time + reg_mgr.ref_advance;

        if(reg_mgr.reg_mode != REG_VOLTAGE)
        {
            ccrun.cycle_time_origin -= reg_mgr.reg_signal->iteration_counter * reg_mgr.iter_period;
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

        regMgrModeSetRT(&reg_mgr, ccpars_ref[ccrun.cyc_sel].reg_mode);

        // Prepare to generate new function

        func_idx = ccpars_ref[ccrun.cyc_sel].function;

        ccrun.fgen_func = funcs[func_idx].fgen_func;
        ccrun.fg_func_pars = funcs[func_idx].fg_pars + funcs[func_idx].size_of_pars * ccrun.cyc_sel;

        ccrun.cycle_end_time = ccpars_global.run_delay + ccpars_ref[ccrun.cyc_sel].fg_meta.duration;

        ccrun.cycle_time_origin = iter_time;

        ccrun.cycle[ccrun.cycle_idx].ref_advance = reg_mgr.ref_advance;
        ccrun.cycle[ccrun.cycle_idx].start_time  = iter_time;

        // Reset pre-function index for when this new function ends

        ccrun.prefunc.idx       = 0;
        ccrun.prefunc.num_ramps = 1;
    }

    return(1); // Return 1 to continue the simulation
}



void ccRun(union sigval sigval)
{
    uint32_t    reg_iteration_counter;
    double      ref_time;
    float       ref;
    bool        use_sim_meas;


    // Adjust time stamp

    ccrun.iter_time.tv_usec += CC_ITER_PERIOD_US;

    if(ccrun.iter_time.tv_usec >= 1000000)
    {
        ccrun.iter_time.tv_sec++;
        ccrun.iter_time.tv_usec -= 1000000;
    }

    ccrun.iter_time_s = (double)ccrun.iter_time.tv_sec + (double)ccrun.iter_time.tv_usec * 1.0E-6;

    // Calculate reference time taking into account the ref advance for the active regulation mode

    ref_time = ccrun.iter_time_s - ccrun.cycle_time_origin + regMgrVar(REG_mgr, REF_ADVANCE);

    // The "real" measurements are invalid while the simulated measurements are good so setting
    // the use_sim_meas is equivalent to indicating to libreg that the measurements are invalid on
    // this iteration.

    use_sim_meas = rand() > (int)(RAND_MAX * ccpars_meas.invalid_probability);

    // Give new measurements to libreg and receive iteration counter (it's zero on regulation iterations)

    reg_iteration_counter = regMgrMeasSetRT(&reg_mgr, REG_OPERATIONAL_RST_PARS, 0, 0, use_sim_meas, false);

    if(ccpars_state.pc == PC_ON && reg_iteration_counter == 0)
    {
        switch(regMgrVar(REG_mgr, REG_MODE))
        {
            case REG_NONE:      ref = 0;                    break;
            case REG_VOLTAGE:   ref = ccpars_direct.v_ref;  break;
            case REG_CURRENT:   ref = ccpars_direct.i_ref;  break;
            case REG_FIELD:     ref = ccpars_direct.b_ref;  break;
        }

        regMgrRegulateRT(&reg_mgr, &ref);
    }

    // Simulate voltage source and load response

    regMgrSimulateRT(&reg_mgr, NULL, 0.0);

    // Check faults and warnings

    ccRunFaultsAndWarnings();

    // Store field regulation signals in log at regulation rate

    if(regMgrVar(REG_mgr, BREG_ITER_COUNTER) == 0)
    {
        ccLogStoreReg(&breg_log, ccrun.iter_time_s);
    }

    // Store current regulation signals in log at regulation rate

    if(regMgrVar(REG_mgr, IREG_ITER_COUNTER) == 0)
    {
        ccLogStoreReg(&ireg_log, ccrun.iter_time_s);
    }

    // Store measurement rate signals in log every iteration

    ccLogStoreMeas(ccrun.iter_time_s);

    // Simulate PC state and Polarity Switch behaviour

    ccSimPcState();
    ccSimPolSwitch();

    if(ccpars_state.pc == PC_ON)
    {
        regMgrModeSetRT(&reg_mgr, ccpars_ref[0].reg_mode);
    }
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

        }

*/
// EOF
