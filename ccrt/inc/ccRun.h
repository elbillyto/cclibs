/*!
 * @file  ccrt/inc/ccRun.h
 *
 * @brief ccrt header file for ccRun.c
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

#ifndef CCRUN_H
#define CCRUN_H

#include <sys/time.h>

#include "pars/global.h"

// GLOBALS should be defined in the source file where global variables should be defined

#ifdef GLOBALS
#define CCRUN_EXT
#else
#define CCRUN_EXT extern
#endif

// Constants

#define MAX_PREFUNCS        3

// Regulation related variables

struct ccrun_vars
{
    struct timeval                  iter_time;                          // Iteration time in seconds and microseconds
    double                          iter_time_s;                        // Iteration time as a double in seconds
    int32_t                         time_till_event_us;                 // Number of iterations for the simulation
    uint32_t                        cycle_idx;                          // Active reference function index
    uint32_t                        cyc_sel;                            // Cycle selector for current cycle

    double                          cycle_start_time;                   // Start time (iter_time) for current cycle
    double                          cycle_duration;                     // Cycle duration including run delay
    enum fg_types                   function;                           // Function type
    bool                            play;                               // Function is to be played
    enum fg_gen_status            (*fgen_func)();                       // Function to generate the active reference
    void                           *fgen_pars;                          // Parameter structure for active reference
    struct fg_limits               *fg_limits;                          // Pointer to NULL or ccrun.fgen_limits
    struct fg_limits                fgen_limits;                        // Pointer to fg_limits (b/i/v)
    struct reg_lim_ref              fg_lim_v_ref;                       // Libreg voltage measurement limits structure for fg converter limits

    struct ccrun_cycle
    {
        uint32_t                    cyc_sel;                            // Cycle selector
        enum reg_rst_source         reg_rst_source;                     // RST parameter source
        double                      start_time;                         // Time of the start of the cycle
        float                       ref_advance;                        // Ref advance used with each function
    } cycle[MAX_CYCLES];

    struct ccrun_invalid_meas
    {
        int                         random_threshold;                   // Threshold for random() for an invalid measurement
        bool                        flag;                               // Invalid measurement flag for logging
    } invalid_meas;

    struct ccrun_prefunc
    {
        uint32_t                    idx;                                // Pre-function stage index (0,1,2,3)
        uint32_t                    num_ramps;                          // Number of pre-function ramps
        float                       final_ref[MAX_PREFUNCS];            // Final ref for each pre-function ramp
        struct fg_ramp              pars;                               // Libfg parameters for pre-function ramps
    } prefunc;

    struct ccrun_polswitch
    {
        uint32_t                    timer;                              // Switch movement time
        enum polswitch_cmd          command;                            // Pol switch command
        bool                        enabled;                            // Pol switch is enabled if POLSWITCH TIMEOUT is not zero
        bool                        automatic;                          // Pol switch is automatically controlled (for Libfg init functions)
        bool                        negative;                           // Pol switch is currently negative (for Libfg init functions)
        enum reg_enabled_disabled   limits_invert;                      // Real-time limites should be inverted (for Libreg)
    } polswitch;
};

CCRUN_EXT struct ccrun_vars ccrun;

// Function prototypes

void    ccRunSimulation         (void);
void    ccRunFuncGen            (void);
void    ccRunFuncGenReverseTime (void);

#endif

// EOF
