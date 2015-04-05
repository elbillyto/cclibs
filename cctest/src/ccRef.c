/*---------------------------------------------------------------------------------------------------------*\
  File:     ccRef.c                                                                     Copyright CERN 2015

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

  Purpose:  Function generation library test functions

  Authors:  Quentin.King@cern.ch
\*---------------------------------------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Include cctest program header files

#include "ccCmds.h"
#include "ccTest.h"
#include "ccRun.h"
#include "ccRef.h"

/*---------------------------------------------------------------------------------------------------------*/
enum FG_func_status ccRefDirectRT(union FG_pars *pars, FG_float func_time, float *ref)
/*---------------------------------------------------------------------------------------------------------*/
{
    float                        prev_rate;
    static float                 prev_ref;
    static float                 next_ref;
    static float                 final_ref;
    static enum FG_func_status    fg_gen_status;

    // Pre-function coast

    if(func_time < 0.0)
    {
        return(FG_GEN_PRE_FUNC);
    }

    // If DIRECT function is already running

    if(pars->table.seg_idx > 0)
    {
        prev_rate = (*ref - prev_ref) / reg_mgr.reg_period;
        prev_ref  = *ref;
    }
    else // DIRECT has not yet started
    {
        // Prepare to force initialisation of first RAMP function

        fg_gen_status = FG_GEN_POST_FUNC;
        prev_rate     = 0.0;
        prev_ref      = *ref;
        final_ref     = 1.0E30;
    }

    // Scan through table to find segment containing the current time

    while(func_time >= pars->table.time[pars->table.seg_idx])      // while time exceeds end of segment
    {
        // If vector is already complete or is now complete

        if(pars->table.seg_idx >= pars->table.num_points || ++pars->table.seg_idx >= pars->table.num_points)
        {
            // Return function running flag from RAMP function

            return(fg_gen_status);
        }
    }

    // If target reference has changed then re-arm the RAMP function

    if(pars->table.ref[pars->table.seg_idx] != next_ref)
    {
        // Initialise RAMP based on reg_mode

        final_ref = next_ref = pars->table.ref[pars->table.seg_idx];

        // Clip reference

        if(reg_mgr.reg_signal->lim_ref.invert_limits == REG_DISABLED)
        {
            if(next_ref > reg_mgr.lim_ref->pos)
            {
                final_ref = reg_mgr.lim_ref->pos;
            }
            else if(next_ref < reg_mgr.lim_ref->neg)
            {
                final_ref = reg_mgr.lim_ref->neg;
            }
        }
        else
        {
            if(next_ref > -reg_mgr.lim_ref->neg)
            {
                final_ref = -reg_mgr.lim_ref->neg;
            }
            else if(next_ref < -reg_mgr.lim_ref->pos)
            {
                final_ref = -reg_mgr.lim_ref->pos;
            }
        }

        // Initialise RAMP to new reference

        fgRampCalc(ccpars_load.pol_swi_auto,
                   ccpars_limits.invert, 
                   func_time - reg_mgr.reg_period,
                   prev_rate,
                   *ref,
                   final_ref,
                   ccpars_default.pars[reg_mgr.reg_mode].acceleration,
                   ccpars_default.pars[reg_mgr.reg_mode].linear_rate,
                   ccpars_default.pars[reg_mgr.reg_mode].deceleration,
                   &ccrun.prefunc.pars);
    }

    fg_gen_status = fgRampRT((FG_pars *)&ccrun.prefunc.pars, func_time, ref);

    return(FG_GEN_DURING_FUNC);
}
/*---------------------------------------------------------------------------------------------------------*/
enum FG_errno ccRefInitPLEP(struct FG_error *fg_error, uint32_t cyc_sel)
/*---------------------------------------------------------------------------------------------------------*/
{
    return(fgPlepInit(  ccrun.fg_func_limits,
                        ccpars_load.pol_swi_auto,
                        ccpars_limits.invert, 
                        ccpars_plep[cyc_sel].initial_ref,
                        ccpars_plep[cyc_sel].final_ref,
                        ccpars_plep[cyc_sel].final_rate,
                        ccpars_plep[cyc_sel].acceleration,
                        ccpars_plep[cyc_sel].linear_rate,
                        ccpars_plep[cyc_sel].exp_tc,
                        ccpars_plep[cyc_sel].exp_final,
                        &ccrun.fg_pars[cyc_sel],
                        fg_error));
}
/*---------------------------------------------------------------------------------------------------------*/
enum FG_errno ccRefInitRAMP(struct FG_error *fg_error, uint32_t cyc_sel)
/*---------------------------------------------------------------------------------------------------------*/
{
    return(fgRampInit(  ccrun.fg_func_limits,
                        ccpars_load.pol_swi_auto,
                        ccpars_limits.invert,
                        ccpars_ramp[cyc_sel].start_time,
                        ccpars_ramp[cyc_sel].initial_ref,
                        ccpars_ramp[cyc_sel].final_ref,
                        ccpars_ramp[cyc_sel].acceleration,
                        ccpars_ramp[cyc_sel].linear_rate,
                        ccpars_ramp[cyc_sel].deceleration,
                        &ccrun.fg_pars[cyc_sel],
                        fg_error));
}
/*---------------------------------------------------------------------------------------------------------*/
enum FG_errno ccRefInitPPPL(struct FG_error *fg_error, uint32_t cyc_sel)
/*---------------------------------------------------------------------------------------------------------*/
{
    return(fgPpplInit(  ccrun.fg_func_limits,
                        ccpars_load.pol_swi_auto,
                        ccpars_limits.invert, 
                        ccpars_pppl[cyc_sel].initial_ref,
                        ccpars_pppl[cyc_sel].acceleration1,
                        pppl_pars[1].num_elements[cyc_sel],
                        ccpars_pppl[cyc_sel].acceleration2,
                        pppl_pars[2].num_elements[cyc_sel],
                        ccpars_pppl[cyc_sel].acceleration3,
                        pppl_pars[3].num_elements[cyc_sel],
                        ccpars_pppl[cyc_sel].rate2,
                        pppl_pars[4].num_elements[cyc_sel],
                        ccpars_pppl[cyc_sel].rate4,
                        pppl_pars[5].num_elements[cyc_sel],
                        ccpars_pppl[cyc_sel].ref4,
                        pppl_pars[6].num_elements[cyc_sel],
                        ccpars_pppl[cyc_sel].duration4,
                        pppl_pars[7].num_elements[cyc_sel],
                        &ccrun.fg_pars[cyc_sel],
                        fg_error));
}
/*---------------------------------------------------------------------------------------------------------*/
enum FG_errno ccRefInitTABLE(struct FG_error *fg_error, uint32_t cyc_sel)
/*---------------------------------------------------------------------------------------------------------*/
{
    return(fgTableInit( ccrun.fg_func_limits,
                        ccpars_load.pol_swi_auto,
                        ccpars_limits.invert, 
                        reg_mgr.iter_period,
                        ccpars_table[cyc_sel].ref,
                        table_pars[0].num_elements[cyc_sel],
                        ccpars_table[cyc_sel].time,
                        table_pars[1].num_elements[cyc_sel],
                        NULL,
                        NULL,
                        &ccrun.fg_pars[cyc_sel],
                        fg_error));
}
/*---------------------------------------------------------------------------------------------------------*/
enum FG_errno ccRefInitSTEPS(struct FG_error *fg_error, uint32_t cyc_sel)
/*---------------------------------------------------------------------------------------------------------*/
{
    return(fgTestInit(  ccrun.fg_func_limits,
                        ccpars_load.pol_swi_auto,
                        ccpars_limits.invert, 
                        FG_TEST_STEPS,
                        ccpars_test[cyc_sel].initial_ref,
                        ccpars_test[cyc_sel].amplitude_pp,
                        ccpars_test[cyc_sel].num_cycles,
                        ccpars_test[cyc_sel].period,
                        false,                                  // is_window_active not used for STEPS
                        false,                                  // is_exp_decay_active not used for STEPS
                        &ccrun.fg_pars[cyc_sel],
                        fg_error));
}
/*---------------------------------------------------------------------------------------------------------*/
enum FG_errno ccRefInitSQUARE(struct FG_error *fg_error, uint32_t cyc_sel)
/*---------------------------------------------------------------------------------------------------------*/
{
    return(fgTestInit(  ccrun.fg_func_limits,
                        ccpars_load.pol_swi_auto,
                        ccpars_limits.invert, 
                        FG_TEST_SQUARE,
                        ccpars_test[cyc_sel].initial_ref,
                        ccpars_test[cyc_sel].amplitude_pp,
                        ccpars_test[cyc_sel].num_cycles,
                        ccpars_test[cyc_sel].period,
                        false,                                  // is_window_active not used for SQUARE
                        false,                                  // is_exp_decay_active not used for SQUARE
                        &ccrun.fg_pars[cyc_sel],
                        fg_error));
}
/*---------------------------------------------------------------------------------------------------------*/
enum FG_errno ccRefInitSINE(struct FG_error *fg_error, uint32_t cyc_sel)
/*---------------------------------------------------------------------------------------------------------*/
{
    return(fgTestInit(  ccrun.fg_func_limits,
                        ccpars_load.pol_swi_auto,
                        ccpars_limits.invert, 
                        FG_TEST_SINE,
                        ccpars_test[cyc_sel].initial_ref,
                        ccpars_test[cyc_sel].amplitude_pp,
                        ccpars_test[cyc_sel].num_cycles,
                        ccpars_test[cyc_sel].period,
                        ccpars_test[cyc_sel].use_window,
                        ccpars_test[cyc_sel].use_exp_decay,
                        &ccrun.fg_pars[cyc_sel],
                        fg_error));
}
/*---------------------------------------------------------------------------------------------------------*/
enum FG_errno ccRefInitCOSINE(struct FG_error *fg_error, uint32_t cyc_sel)
/*---------------------------------------------------------------------------------------------------------*/
{
    return(fgTestInit(  ccrun.fg_func_limits,
                        ccpars_load.pol_swi_auto,
                        ccpars_limits.invert, 
                        FG_TEST_COSINE,
                        ccpars_test[cyc_sel].initial_ref,
                        ccpars_test[cyc_sel].amplitude_pp,
                        ccpars_test[cyc_sel].num_cycles,
                        ccpars_test[cyc_sel].period,
                        ccpars_test[cyc_sel].use_window,
                        ccpars_test[cyc_sel].use_exp_decay,
                        &ccrun.fg_pars[cyc_sel],
                        fg_error));
}
/*---------------------------------------------------------------------------------------------------------*/
enum FG_errno ccRefInitLTRIM(struct FG_error *fg_error, uint32_t cyc_sel)
/*---------------------------------------------------------------------------------------------------------*/
{
    return(fgTrimInit(  ccrun.fg_func_limits,
                        ccpars_load.pol_swi_auto,
                        ccpars_limits.invert, 
                        FG_TRIM_LINEAR,
                        ccpars_trim[cyc_sel].initial_ref,
                        ccpars_trim[cyc_sel].final_ref,
                        ccpars_trim[cyc_sel].duration,
                        &ccrun.fg_pars[cyc_sel],
                        fg_error));
}
/*---------------------------------------------------------------------------------------------------------*/
enum FG_errno ccRefInitCTRIM(struct FG_error *fg_error, uint32_t cyc_sel)
/*---------------------------------------------------------------------------------------------------------*/
{
    return(fgTrimInit(  ccrun.fg_func_limits,
                        ccpars_load.pol_swi_auto,
                        ccpars_limits.invert, 
                        FG_TRIM_CUBIC,
                        ccpars_trim[cyc_sel].initial_ref,
                        ccpars_trim[cyc_sel].final_ref,
                        ccpars_trim[cyc_sel].duration,
                        &ccrun.fg_pars[cyc_sel],
                        fg_error));
}
/*---------------------------------------------------------------------------------------------------------*/
enum FG_errno ccRefInitPULSE(struct FG_error *fg_error, uint32_t cyc_sel)
/*---------------------------------------------------------------------------------------------------------*/
{
    return(fgPulseInit( ccrun.fg_func_limits,
                        ccpars_load.pol_swi_auto,
                        ccpars_limits.invert, 
                        ccpars_pulse[cyc_sel].ref,
                        ccpars_pulse[cyc_sel].acceleration,
                        ccpars_pulse[cyc_sel].linear_rate,
                        ccpars_pulse[cyc_sel].duration,
                        &ccrun.fg_pars[cyc_sel],
                        fg_error));
}

// EOF
