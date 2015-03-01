/*!
 * @file  ccrt/inc/ccLog.h
 *
 * @brief ccrt header file for ccLog.c
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

#ifndef CCLOG_H
#define CCLOG_H

#include <stdint.h>

#include "ccPars.h"

// GLOBALS should be defined in the source file where global variables should be defined

#ifdef GLOBALS
#define CCLOG_EXT
#else
#define CCLOG_EXT extern
#endif

// Signal structures

struct cclog_ana_sigs
{
    char                       *name;                   // Signal name
    bool                        is_trailing_step;       // Trailing step flag
    float const                *source;                 // Pointer to source of analogue signal
    uint32_t                    num_bad_values;         // Counter for bad values
    float                       buf[CC_LOG_LENGTH];     // Signal circular buffer
};

struct cclog
{
    struct cclog_ana_sigs      *ana_sigs;               // Pointer to ana_sigs array
    uint32_t                    num_ana_signals;        // Number of signals in ana_sigs
    int32_t                     last_sample_index;      // Index of most recent sample
    double                      last_sample_time;       // Time stamp for most recent sample
};

// RMS currents not directly available in struct conv. An sqrt() is needed first.

CCLOG_EXT float    i_rms;                               // RMS current in converter
CCLOG_EXT float    i_rms_load;                          // RMS current in load

// Field/Current regulation rate analog log signals

enum cclog_ana_reg_idx
{
    ANA_REF,                  // Reference
    ANA_REF_LIMITED,          // Reference after limits applied
    ANA_REF_RST,              // Reference stored in RST history (limited and after back-calculation)
    ANA_REF_OPENLOOP,         // Reference stored in open loop history (after back-calculation)

    NUM_REG_SIGNALS
};

// Field regulation log

CCLOG_EXT struct cclog_ana_sigs ana_breg_sigs[]
#ifdef GLOBALS
= { // Name                  Trailing_step             Source
    { "B_REF",                  true,           &regMgrVar(conv, BREG_REF         )    },
    { "B_REF_LIMITED",          true,           &regMgrVar(conv, BREG_REF_LIMITED )    },
    { "B_REF_RST",              true,           &regMgrVar(conv, BREG_REF_RST     )    },
    { "B_REF_OPENLOOP",         true,           &regMgrVar(conv, BREG_REF_OPENLOOP)    },
}
#endif
;

CCLOG_EXT struct cclog breg_log
#ifdef GLOBALS
= {
    ana_breg_sigs,
    NUM_REG_SIGNALS,
}
#endif
;

// Current regulation log

CCLOG_EXT struct cclog_ana_sigs ana_ireg_sigs[]
#ifdef GLOBALS
= { // Name                  Trailing_step             Source
    { "I_REF",                  true,           &regMgrVar(conv, IREG_REF         )   },
    { "I_REF_LIMITED",          true,           &regMgrVar(conv, IREG_REF_LIMITED )   },
    { "I_REF_RST",              true,           &regMgrVar(conv, IREG_REF_RST     )   },
    { "I_REF_OPENLOOP",         true,           &regMgrVar(conv, IREG_REF_OPENLOOP)   },
}
#endif
;

CCLOG_EXT struct cclog ireg_log
#ifdef GLOBALS
= {
    ana_ireg_sigs,
    NUM_REG_SIGNALS,
}
#endif
;

// Measurement rate analogue log signals

enum cclog_ana_meas_idx
{
    ANA_B_MEAS,                 // Unfiltered field measurement
    ANA_B_MEAS_FLTR,            // Filtered field measurement
    ANA_B_MEAS_EXTR,            // Extrapolated field measurement

    ANA_I_RMS,                  // Filtered RMS circuit current for converter protection
    ANA_I_RMS_LOAD,             // Filtered RMS circuit current for load protections
    ANA_I_MEAS,                 // Unfiltered measured current
    ANA_I_MEAS_FLTR,            // Filtered measured current
    ANA_I_MEAS_EXTR,            // Extrapolated measured current

    ANA_V_REF,                  // Voltage reference
    ANA_V_REF_SAT,              // Voltage reference after magnet saturation compensation applied
    ANA_V_REF_LIMITED,          // Voltage reference after saturation compensation and limits applied
    ANA_V_MEAS,                 // Measured voltage

    ANA_B_ERR,                  // Field regulation error
    ANA_I_ERR,                  // Current regulation error

    ANA_MAX_ABS_B_ERR,          // Max absolute field regulation error
    ANA_MAX_ABS_I_ERR,          // Max absolute current regulation error

    NUM_ANA_SIGNALS
};

CCLOG_EXT struct cclog_ana_sigs ana_meas_sigs[]     // IMPORTANT: This must be in the same order as enum cclog_ana_meas_idx (above)
#ifdef GLOBALS
= { // Name                  Trailing_step               Source
    { "B_MEAS",                 false,          &regMgrVar(conv, MEAS_B_UNFILTERED)        },
    { "B_MEAS_FLTR",            false,          &regMgrVar(conv, MEAS_B_FILTERED)          },
    { "B_MEAS_EXTR",            false,          &regMgrVar(conv, MEAS_B_EXTRAPOLATED)      },

    { "I_RMS",                  false,          &i_rms                                      },
    { "I_RMS_LOAD",             false,          &i_rms_load                                 },
    { "I_MEAS",                 false,          &regMgrVar(conv, MEAS_I_UNFILTERED)        },
    { "I_MEAS_FLTR",            false,          &regMgrVar(conv, MEAS_I_FILTERED)          },
    { "I_MEAS_EXTR",            false,          &regMgrVar(conv, MEAS_I_EXTRAPOLATED)      },

    { "V_REF",                  true,           &regMgrVar(conv, V_REF)                    },
    { "V_REF_SAT",              true,           &regMgrVar(conv, V_REF_SAT)                },
    { "V_REF_LIMITED",          true,           &regMgrVar(conv, V_REF_LIMITED)            },
    { "V_MEAS",                 false,          &regMgrVar(conv, MEAS_V)                   },

    { "B_ERR",                  true,           &regMgrVar(conv, BREG_ERR)                 },
    { "I_ERR",                  true,           &regMgrVar(conv, IREG_ERR)                 },

    { "MAX_ABS_B_ERR",          true,           &regMgrVar(conv, BREG_MAX_ABS_ERR)         },
    { "MAX_ABS_I_ERR",          true,           &regMgrVar(conv, IREG_MAX_ABS_ERR)         },
}
#endif
;

// Measurement rate log structure

CCLOG_EXT struct cclog meas_log
#ifdef GLOBALS
= {
    ana_meas_sigs,
    NUM_ANA_SIGNALS,
}
#endif
;

// Function declarations

void     ccLogStoreReg          (struct cclog *log, double time);
void     ccLogStoreMeas         (double time);
uint32_t ccLogReportBadValues   (struct cclog *log);

#endif
// EOF
