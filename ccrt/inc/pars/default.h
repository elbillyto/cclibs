/*!
 * @file  ccrt/inc/pars/default.h
 *
 * @brief ccrt DEFAULT parameters
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

#ifndef CCPARS_DEFAULT_H
#define CCPARS_DEFAULT_H

#include "ccPars.h"

// GLOBALS should be defined in the source file where global variables should be defined

#ifdef GLOBALS
#define CCPARS_DEFAULT_EXT
#else
#define CCPARS_DEFAULT_EXT extern
#endif

// Default parameters structure

struct ccpars_default_pars
{
    float                       acceleration;         // Default acceleration
    float                       deceleration;         // Default deceleration
    float                       linear_rate ;         // Default linear_rate
};

struct ccpars_default
{
    struct ccpars_default_pars  pars[REG_NONE];       // Default parameters for REG_VOLTAGE, REG_CURRENT, REG_FIELD
};

CCPARS_DEFAULT_EXT struct ccpars_default ccpars_default
#ifdef GLOBALS
= {// Default value       Parameter
    {
        {
            1.0,       // DEFAULT V_ACCELERATION
            1.0,       // DEFAULT V_DECELERATION
            0.0,       // DEFAULT V_LINEAR_RATE
        },
        {
            1.0,       // DEFAULT I_ACCELERATION
            1.0,       // DEFAULT I_DECELERATION
            0.0,       // DEFAULT I_LINEAR_RATE
        },
        {
            1.0,       // DEFAULT B_ACCELERATION
            1.0,       // DEFAULT B_DECELERATION
            0.0,       // DEFAULT B_LINEAR_RATE
        }
    },
}
#endif
;

// Default parameters description structure

CCPARS_DEFAULT_EXT struct ccpars default_pars[]
#ifdef GLOBALS
= {// "Signal name"       type,  max_n_els, *enum,        *value,                                  num_defaults,cyc_sel_step,flags
    { "V_ACCELERATION",   PAR_FLOAT, 1,      NULL, { .f = &ccpars_default.pars[REG_VOLTAGE].acceleration   }, 1,      0,     PARS_RW|PARS_REG|PARS_CFG },
    { "V_DECELERATION",   PAR_FLOAT, 1,      NULL, { .f = &ccpars_default.pars[REG_VOLTAGE].deceleration   }, 1,      0,     PARS_RW|PARS_REG|PARS_CFG },
    { "V_LINEAR_RATE",    PAR_FLOAT, 1,      NULL, { .f = &ccpars_default.pars[REG_VOLTAGE].linear_rate    }, 1,      0,     PARS_RW|PARS_REG|PARS_CFG },
    { "I_ACCELERATION",   PAR_FLOAT, 1,      NULL, { .f = &ccpars_default.pars[REG_CURRENT].acceleration   }, 1,      0,     PARS_RW|PARS_REG|PARS_CFG },
    { "I_DECELERATION",   PAR_FLOAT, 1,      NULL, { .f = &ccpars_default.pars[REG_CURRENT].deceleration   }, 1,      0,     PARS_RW|PARS_REG|PARS_CFG },
    { "I_LINEAR_RATE",    PAR_FLOAT, 1,      NULL, { .f = &ccpars_default.pars[REG_CURRENT].linear_rate    }, 1,      0,     PARS_RW|PARS_REG|PARS_CFG },
    { "B_ACCELERATION",   PAR_FLOAT, 1,      NULL, { .f = &ccpars_default.pars[REG_FIELD].acceleration     }, 1,      0,     PARS_RW|PARS_REG|PARS_CFG },
    { "B_DECELERATION",   PAR_FLOAT, 1,      NULL, { .f = &ccpars_default.pars[REG_FIELD].deceleration     }, 1,      0,     PARS_RW|PARS_REG|PARS_CFG },
    { "B_LINEAR_RATE",    PAR_FLOAT, 1,      NULL, { .f = &ccpars_default.pars[REG_FIELD].linear_rate      }, 1,      0,     PARS_RW|PARS_REG|PARS_CFG },
    { NULL }
}
#endif
;

#endif
// EOF

