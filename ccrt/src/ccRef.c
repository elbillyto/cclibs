/*!
 * @file  ccrt/scr/ccRef.c
 *
 * @brief ccrt reference generation processing functions
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
#include <string.h>

// Include cctest program header files

#include "ccCmds.h"
#include "ccRt.h"
#include "ccRun.h"
#include "ccRef.h"
#include "ccSim.h"



enum fg_error ccRefInitPLEP(struct fg_meta *fg_meta, uint32_t cyc_sel)
{
    return(fgPlepInit(  &ccrun.fg_limits,
                        ccsim.polswitch.automatic,
                        ccsim.polswitch.negative,
                        ccpars_global.run_delay,
                        ccpars_plep[cyc_sel].initial_ref,
                        ccpars_plep[cyc_sel].final_ref,
                        0.0,                                    // final_rate is not a user settable parameters in ccrt
                        ccpars_plep[cyc_sel].acceleration,
                        ccpars_plep[cyc_sel].linear_rate,
                        ccpars_plep[cyc_sel].exp_tc,
                        ccpars_plep[cyc_sel].exp_final,
                        &fg_plep[cyc_sel],
                        fg_meta));
}



enum fg_error ccRefInitRAMP(struct fg_meta *fg_meta, uint32_t cyc_sel)
{
    return(fgRampInit(  &ccrun.fg_limits,
                        ccsim.polswitch.automatic,
                        ccsim.polswitch.negative,
                        ccpars_global.run_delay,
                        ccpars_ramp[cyc_sel].initial_ref,
                        ccpars_ramp[cyc_sel].final_ref,
                        ccpars_ramp[cyc_sel].acceleration,
                        ccpars_ramp[cyc_sel].linear_rate,
                        ccpars_ramp[cyc_sel].deceleration,
                        &fg_ramp[cyc_sel],
                        fg_meta));
}



enum fg_error ccRefInitPPPL(struct fg_meta *fg_meta, uint32_t cyc_sel)
{
    return(fgPpplInit(  &ccrun.fg_limits,
                        ccsim.polswitch.automatic,
                        ccsim.polswitch.negative,
                        ccpars_global.run_delay,
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
                        &fg_pppl[cyc_sel],
                        fg_meta));
}



enum fg_error ccRefInitTABLE(struct fg_meta *fg_meta, uint32_t cyc_sel)
{
    return(fgTableInit( &ccrun.fg_limits,
                        ccsim.polswitch.automatic,
                        ccsim.polswitch.negative,
                        ccpars_global.run_delay,
                        reg_mgr.iter_period,
                        ccpars_table[cyc_sel].ref,
                        table_pars[0].num_elements[cyc_sel],
                        ccpars_table[cyc_sel].time,
                        table_pars[1].num_elements[cyc_sel],
                        &fg_table[cyc_sel],
                        fg_meta));
    return(EXIT_SUCCESS);
}



enum fg_error ccRefInitSTEPS(struct fg_meta *fg_meta, uint32_t cyc_sel)
{
    return(fgTestInit(  &ccrun.fg_limits,
                        ccsim.polswitch.automatic,
                        ccsim.polswitch.negative,
                        ccpars_global.run_delay,
                        FG_TEST_STEPS,
                        ccpars_test[cyc_sel].initial_ref,
                        ccpars_test[cyc_sel].amplitude_pp,
                        ccpars_test[cyc_sel].num_cycles,
                        ccpars_test[cyc_sel].period,
                        false,                                  // is_window_active not used for STEPS
                        false,                                  // is_exp_decay_active not used for STEPS
                        &fg_test[cyc_sel],
                        fg_meta));
}



enum fg_error ccRefInitSQUARE(struct fg_meta *fg_meta, uint32_t cyc_sel)
{
    return(fgTestInit(  &ccrun.fg_limits,
                        ccsim.polswitch.automatic,
                        ccsim.polswitch.negative,
                        ccpars_global.run_delay,
                        FG_TEST_SQUARE,
                        ccpars_test[cyc_sel].initial_ref,
                        ccpars_test[cyc_sel].amplitude_pp,
                        ccpars_test[cyc_sel].num_cycles,
                        ccpars_test[cyc_sel].period,
                        false,                                  // is_window_active not used for SQUARE
                        false,                                  // is_exp_decay_active not used for SQUARE
                        &fg_test[cyc_sel],
                        fg_meta));
}



enum fg_error ccRefInitSINE(struct fg_meta *fg_meta, uint32_t cyc_sel)
{
    return(fgTestInit(  &ccrun.fg_limits,
                        ccsim.polswitch.automatic,
                        ccsim.polswitch.negative,
                        ccpars_global.run_delay,
                        FG_TEST_SINE,
                        ccpars_test[cyc_sel].initial_ref,
                        ccpars_test[cyc_sel].amplitude_pp,
                        ccpars_test[cyc_sel].num_cycles,
                        ccpars_test[cyc_sel].period,
                        ccpars_test[cyc_sel].use_window,
                        ccpars_test[cyc_sel].use_exp_decay,
                        &fg_test[cyc_sel],
                        fg_meta));
}



enum fg_error ccRefInitCOSINE(struct fg_meta *fg_meta, uint32_t cyc_sel)
{
    return(fgTestInit(  &ccrun.fg_limits,
                        ccsim.polswitch.automatic,
                        ccsim.polswitch.negative,
                        ccpars_global.run_delay,
                        FG_TEST_COSINE,
                        ccpars_test[cyc_sel].initial_ref,
                        ccpars_test[cyc_sel].amplitude_pp,
                        ccpars_test[cyc_sel].num_cycles,
                        ccpars_test[cyc_sel].period,
                        ccpars_test[cyc_sel].use_window,
                        ccpars_test[cyc_sel].use_exp_decay,
                        &fg_test[cyc_sel],
                        fg_meta));
}



enum fg_error ccRefInitLTRIM(struct fg_meta *fg_meta, uint32_t cyc_sel)
{
    return(fgTrimInit(  &ccrun.fg_limits,
                        ccsim.polswitch.automatic,
                        ccsim.polswitch.negative,
                        ccpars_global.run_delay,
                        FG_TRIM_LINEAR,
                        ccpars_trim[cyc_sel].initial_ref,
                        ccpars_trim[cyc_sel].final_ref,
                        ccpars_trim[cyc_sel].duration,
                        &fg_trim[cyc_sel],
                        fg_meta));
}



enum fg_error ccRefInitCTRIM(struct fg_meta *fg_meta, uint32_t cyc_sel)
{
    return(fgTrimInit(  &ccrun.fg_limits,
                        ccsim.polswitch.automatic,
                        ccsim.polswitch.negative,
                        ccpars_global.run_delay,
                        FG_TRIM_CUBIC,
                        ccpars_trim[cyc_sel].initial_ref,
                        ccpars_trim[cyc_sel].final_ref,
                        ccpars_trim[cyc_sel].duration,
                        &fg_trim[cyc_sel],
                        fg_meta));
}



enum fg_error ccRefInitPULSE(struct fg_meta *fg_meta, uint32_t cyc_sel)
{
    uint32_t    index = ccpars_pulse[cyc_sel].index;

    if(index >= pulse_pars[PULSE_TIME       ].num_elements[cyc_sel] ||
       index >= pulse_pars[PULSE_DURATION   ].num_elements[cyc_sel] ||
       index >= pulse_pars[PULSE_REF        ].num_elements[cyc_sel] ||
       index >= pulse_pars[PULSE_LINEAR_RATE].num_elements[cyc_sel])
    {
        fgResetMeta(fg_meta, NULL, 0.0, 0.0);
        fg_meta->fg_error = FG_BAD_ARRAY_LEN;
        return(FG_BAD_ARRAY_LEN);
    }

    return(fgPulseInit( &ccrun.fg_limits,
                        ccsim.polswitch.automatic,
                        ccsim.polswitch.negative,
                        ccpars_global.run_delay + ccpars_pulse[cyc_sel].time[index],
                        ccpars_pulse[cyc_sel].ref[index],
                        ccpars_pulse[cyc_sel].linear_rate[index],
                        ccpars_pulse[cyc_sel].duration[index],
                        &fg_pulse[cyc_sel],
                        fg_meta));
}

// EOF
