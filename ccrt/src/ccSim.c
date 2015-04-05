/*!
 * @file  ccrt/scr/ccSim.c
 *
 * @brief ccrt real-time simulation functions
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

#include <math.h>

// Include libreg header files

#include "libreg_vars.h"

// Include ccrt program header files

#include "ccCmds.h"
#include "ccRt.h"
#include "ccFile.h"
#include "ccParse.h"
#include "ccRef.h"
#include "ccLog.h"
#include "ccRun.h"
#include "ccSim.h"



void ccSimPcState(void)
{
    // If any fault is active then immediately switch to FAULT state

    if(ccrun.fault)
    {
        ccpars_state.pc = PC_FAULT;
    }
    else
    {
        // Delay switching on but not switching off

        if(ccpars_pc.state != ccpars_state.pc)
        {
            ccsim.pcstate.timer += regMgrVar(REG_mgr,ITER_PERIOD);

            if(ccpars_pc.state == PC_OFF || ccsim.pcstate.timer > 1.5)
            {
                ccpars_state.pc = ccpars_pc.state;
                ccsim.pcstate.timer = 0.0;
            }
        }
    }
}



void ccSimPolSwitch(void)
{
    // Run PolSwitch state machine

    switch(ccpars_state.polswitch)
    {
        case POLSWITCH_NONE:

            ccsim.polswitch.timer = 0.0;

            if(regMgrVar(REG_mgr,REG_MODE) == REG_NONE &&
               ccpars_polswitch.timeout > 0.0)
            {
                switch(ccpars_polswitch.command)
                {
                    case POLSWITCH_CMD_AUTOMATIC:
                    case POLSWITCH_CMD_POSITIVE:

                        ccpars_state.polswitch = ccsim.polswitch.target_state = POLSWITCH_POSITIVE;
                        break;

                    case POLSWITCH_CMD_NEGATIVE:

                        ccpars_state.polswitch = ccsim.polswitch.target_state = POLSWITCH_NEGATIVE;
                        break;
                }
            }
            break;

        case POLSWITCH_POSITIVE:

            if(ccpars_polswitch.timeout <= 0.0)
            {
                ccpars_state.polswitch = POLSWITCH_NONE;
            }
            else if(regMgrVar(REG_mgr,REG_MODE) == REG_NONE &&
              (ccpars_polswitch.command == POLSWITCH_CMD_NEGATIVE || ccsim.polswitch.target_state == POLSWITCH_NEGATIVE))
            {
                ccsim.polswitch.target_state = POLSWITCH_NEGATIVE;
                ccpars_state.polswitch  = POLSWITCH_MOVING;
            }
            break;

        case POLSWITCH_NEGATIVE:

            if(ccpars_polswitch.timeout <= 0.0)
            {
                ccpars_state.polswitch = POLSWITCH_NONE;
            }
            else if(regMgrVar(REG_mgr,REG_MODE) == REG_NONE &&
              (ccpars_polswitch.command == POLSWITCH_CMD_POSITIVE || ccsim.polswitch.target_state == POLSWITCH_POSITIVE))
            {
                ccsim.polswitch.target_state = POLSWITCH_POSITIVE;
                ccpars_state.polswitch  = POLSWITCH_MOVING;
            }
            break;

        case POLSWITCH_MOVING:

            ccsim.polswitch.timer += regMgrVar(REG_mgr,ITER_PERIOD);

            if(ccsim.polswitch.timer > ccpars_polswitch.timeout)
            {
                ccsim.polswitch.timer   = 0.0;
                ccpars_state.polswitch  = POLSWITCH_FAULT;
            }

            if(ccsim.polswitch.timer > ccpars_polswitch.sim_delay)
            {
                ccpars_state.polswitch = ccsim.polswitch.target_state;
                ccsim.polswitch.timer  = 0.0;
            }
            break;

        case POLSWITCH_FAULT:

            // Simulate pol switch fault being active for 5s to make it visible when the converter is off

            ccsim.polswitch.timer += regMgrVar(REG_mgr,ITER_PERIOD);

            if(ccsim.polswitch.timer > 5.0)
            {
                ccpars_state.polswitch = POLSWITCH_NONE;
            }
            break;
    }

    // Set flags

    ccsim.polswitch.enabled = (ccpars_state.polswitch != POLSWITCH_NONE);

    ccsim.polswitch.automatic = ccsim.polswitch.enabled && ccpars_polswitch.command == POLSWITCH_CMD_AUTOMATIC;

    ccsim.polswitch.negative  = ccpars_state.polswitch == POLSWITCH_NEGATIVE;

    ccsim.polswitch.limits_invert = ccsim.polswitch.negative ? REG_ENABLED : REG_DISABLED;
}



void ccSimSuperCycle(union sigval sigval)
{
    uint32_t cyc_sel     = ccrun.supercycle.cyc_sel;
    uint32_t ref_cyc_sel = ccrun.supercycle.ref_cyc_sel;
    uint32_t cyc_len     = ccrun.supercycle.seconds_remaining;
    uint32_t cyc_idx     = ccrun.supercycle.cyc_idx;
    uint32_t num_cycles  = global_pars[GLOBAL_CYCLE_SELECTOR].num_elements[0];

    if(cyc_len == 0)
    {
        uint32_t cycle_selector;    // CCLL where CC=cyc_sel  LL=duration in seconds

        if(++cyc_idx >= num_cycles)
        {
            cyc_idx = 0;
        }

        cycle_selector = ccpars_global.cycle_selector[cyc_idx];

        if(cycle_selector == 0)
        {
            cyc_idx = 0;
            cycle_selector = ccpars_global.cycle_selector[cyc_idx];
        }

        cyc_sel = cycle_selector / 100;
        cyc_len = cycle_selector % 100;

        if(cyc_len == 0)
        {
            cyc_len = 1;
        }
    }

    ref_cyc_sel = cyc_sel;

    if(cyc_sel > 0 && cyc_sel == ccpars_global.test_cyc_sel && ccpars_global.test_ref_cyc_sel > 0)
    {
        ref_cyc_sel = ccpars_global.test_ref_cyc_sel;
    }

    // Assume that basic period is 1s

    ccrun.supercycle.seconds_remaining = --cyc_len;

    // Update supercycle information using mutex protection

    pthread_mutex_lock(&ccrun.supercycle.mutex);

    ccrun.supercycle.cyc_idx     = cyc_idx;
    ccrun.supercycle.cyc_sel     = cyc_sel;
    ccrun.supercycle.ref_cyc_sel = ref_cyc_sel;

    pthread_mutex_unlock(&ccrun.supercycle.mutex);

    // On last second of the cycle, notify about the next cycle

    if(ccrun.supercycle.seconds_remaining == 0)
    {
    // Call libref function to declare next event - when in CYCLING or TO_CYCLING states
    }
}

// EOF
