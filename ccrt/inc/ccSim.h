/*!
 * @file  ccrt/inc/ccSim.h
 *
 * @brief ccrt header file for ccSim.c
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

#ifndef CCSIM_H
#define CCSIM_H

#include <sys/time.h>

#include "pars/global.h"

// GLOBALS should be defined in the source file where global variables should be defined

#ifdef GLOBALS
#define CCSIM_EXT
#else
#define CCSIM_EXT extern
#endif

// Simulation related variables

struct ccsim_vars
{
    struct ccsim_pc_state
    {
        float                       timer;                              // Converter on/off timer (s)
    } pcstate;

    struct ccsim_polswitch
    {
        float                       timer;                              // Switch movement time (s)
        enum state_polswitch        target_state;                       // Pol switch target state
        bool                        enabled;                            // Pol switch is enabled if POLSWITCH TIMEOUT is not zero
        bool                        automatic;                          // Pol switch is automatically controlled (for Libfg init functions)
        bool                        negative;                           // Pol switch is currently negative (for Libfg init functions)
        enum reg_enabled_disabled   limits_invert;                      // Real-time limites should be inverted (for Libreg)
    } polswitch;
};

CCSIM_EXT struct ccsim_vars ccsim;

// Function prototypes

void    ccSimPcState            (void);
void    ccSimPolSwitch          (void);
void    ccSimSuperCycle         (union sigval sigval);

#endif

// EOF
