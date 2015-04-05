/*!
 * @file  ccrt/inc/pars/global.h
 *
 * @brief ccrt GLOBAL parameters
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


#ifndef CCPARS_GLOBAL_H
#define CCPARS_GLOBAL_H

#include "ccPars.h"

// GLOBALS should be defined in the source file where global variables should be defined

#ifdef GLOBALS
#define CCPARS_GLOBAL_EXT
#else
#define CCPARS_GLOBAL_EXT extern
#endif

// Constants

#define MAX_CYCLES  16

// Regulation error rate control enum comes from libreg

CCPARS_GLOBAL_EXT struct CCpars_enum enum_reg_err_rate[]
#ifdef GLOBALS
= {
    { REG_ERR_RATE_REGULATION,   "REGULATION"     },
    { REG_ERR_RATE_MEASUREMENT,  "MEASUREMENT"    },
    { 0,                          NULL            },
}
#endif
;

// Global parameters structure

struct CCpars_global
{
    uint32_t                    cycle_selector[MAX_CYCLES]; // Cycle selectors: CCLL   CC=Cycle selector  LL=Length in seconds
    uint32_t                    test_cyc_sel;               // Cycle selector on which to use test RST parameters
    uint32_t                    test_ref_cyc_sel;           // Cycle selector for reference function when playing test_cyc_sel
    float                       run_delay;                  // Delay given to libfg for each function
    enum REG_err_rate           reg_err_rate;               // Regulation error rate control
    float                       log_duration;               // Duration of log saved with LOG command in seconds
};

CCPARS_GLOBAL_EXT struct CCpars_global ccpars_global
#ifdef GLOBALS
= {//  Default value               Parameter
    { 0 }                  ,    // GLOBAL CYCLE_SELECTOR
    0                      ,    // GLOBAL TEST_CYC_SEL
    0                      ,    // GLOBAL TEST_REF_CYC_SEL
    0.0                    ,    // GLOBAL RUN_DELAY
    REG_ERR_RATE_REGULATION,    // GLOBAL REG_ERR_RATE
    2.0                    ,    // GLOBAL LOG_DURATION
}
#endif
;

// Global parameter indexes

enum global_pars_index_enum
{
    GLOBAL_CYCLE_SELECTOR    ,
    GLOBAL_TEST_CYC_SEL      ,
    GLOBAL_TEST_REF_CYC_SEL  ,
    GLOBAL_LOG_RUN_DELAY     ,
    GLOBAL_REG_ERR_RATE      ,
    GLOBAL_LOG_DURATION      ,
};

CCPARS_GLOBAL_EXT struct CCpars global_pars[]
#ifdef GLOBALS
= {// "Signal name"      type,         max_n_els, *enum,                         *value,            num_defaults,cyc_sel_step,flags
    { "CYCLE_SELECTOR",  PAR_UNSIGNED, MAX_CYCLES, NULL,                  { .u =  CCpars_global.cycle_selector   }, 1, 0, PARS_RW|PARS_CFG          },
    { "TEST_CYC_SEL",    PAR_UNSIGNED, 1,          NULL,                  { .u = &CCpars_global.test_cyc_sel     }, 1, 0, PARS_RW|PARS_CFG          },
    { "TEST_REF_CYC_SEL",PAR_UNSIGNED, 1,          NULL,                  { .u = &CCpars_global.test_ref_cyc_sel }, 1, 0, PARS_RW|PARS_CFG          },
    { "RUN_DELAY",       PAR_FLOAT,    1,          NULL,                  { .f = &CCpars_global.run_delay        }, 1, 0, PARS_RW|PARS_CFG          },
    { "REG_ERR_RATE",    PAR_ENUM,     1,          enum_reg_err_rate,     { .u = &CCpars_global.REG_err_rate     }, 1, 0, PARS_RW|PARS_CFG|PARS_REG },
    { "LOG_DURATION",    PAR_FLOAT,    1,          NULL,                  { .f = &CCpars_global.log_duration     }, 1, 0, PARS_RW|PARS_CFG          },
    { NULL }
}
#endif
;

#endif
// EOF
