/*---------------------------------------------------------------------------------------------------------*\
  File:     cctest/inc/ccLog.h                                                          Copyright CERN 2015

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

  Purpose:  Header file for cctest program ccLog.c
\*---------------------------------------------------------------------------------------------------------*/

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
    float                      *buf;                    // Signal buffer (for FLOT output)
    float                      *source;                 // Pointer to source of analogue signal
    float                       time_offset;            // Time offset for trace
    float                       value;                  // Signal value
    uint32_t                    num_bad_values;         // Counter for bad values
    bool                        is_enabled;             // Signal in use flag
};

struct cclog_dig_sigs
{
    char                       *name;                   // Signal name
    uint8_t                    *buf;                    // Signal buffer (for FLOT output)
    bool                       *source;                 // Pointer to source of digital signal
    bool                        is_enabled;             // Signal in use flag
    uint8_t                     value;                  // Signal value
};

struct cclog
{
    struct cclog_ana_sigs      *ana_sigs;               // Pointer to ana_sigs array
    uint32_t                    num_ana_signals;        // Number of signals in ana_sigs
    struct cclog_dig_sigs      *dig_sigs;               // Pointer to dig_sigs array (only used for measurement rate signals)
    uint32_t                    num_dig_signals;        // Number of signals in ana_sigs
    int32_t                     num_samples;            // Number of samples recorded
    int32_t                     last_sample_index;      // Index of most recent sample
    double                      last_sample_time;       // Time stamp for most recent sample
};

// Field/Current regulation rate analog log signals

enum cclog_ana_reg_idx
{
    ANA_MEAS_REG,             // Measurement used for regulation
    ANA_REF,                  // Reference
    ANA_REF_LIMITED,          // Reference after limits applied
    ANA_REF_RST,              // Reference stored in RST history (limited and after back-calculation)
    ANA_REF_OPENLOOP,         // Reference stored in open loop history (after back-calculation)
    ANA_TRACK_DELAY,          // Measured track delay

    NUM_REG_SIGNALS
};

// Field regulation log

CCLOG_EXT struct cclog_ana_sigs ana_breg_sigs[]
#ifdef GLOBALS
= { // Name                  Trailing_step
    { "B_MEAS_REG",             true },
    { "B_REF",                  true },
    { "B_REF_LIMITED",          true },
    { "B_REF_RST",              true },
    { "B_REF_OPENLOOP",         true },
    { "B_TRACK_DELAY",          true },
}
#endif
;

CCLOG_EXT struct cclog breg_log
#ifdef GLOBALS
= {
    ana_breg_sigs,
    NUM_REG_SIGNALS,
    NULL,                       // No digital signals at field regulation rate
    0,
}
#endif
;


// Current regulation log

CCLOG_EXT struct cclog_ana_sigs ana_ireg_sigs[]
#ifdef GLOBALS
= { // Name                  Trailing_step
    { "I_MEAS_REG",             true },
    { "I_REF",                  true },
    { "I_REF_LIMITED",          true },
    { "I_REF_RST",              true },
    { "I_REF_OPENLOOP",         true },
    { "I_TRACK_DELAY",          true },
}
#endif
;

CCLOG_EXT struct cclog ireg_log
#ifdef GLOBALS
= {
    ana_ireg_sigs,
    NUM_REG_SIGNALS,
    NULL,                       // No digital signals at current regulation rate
    0,
}
#endif
;

// Measurement rate analogue log signals

enum cclog_ana_meas_idx
{
    ANA_B_MAGNET,               // Field in the magnet
    ANA_B_MEAS,                 // Unfiltered field measurement
    ANA_B_MEAS_FLTR,            // Filtered field measurement
    ANA_B_MEAS_EXTR,            // Extrapolated field measurement
    ANA_B_REF_DELAYED,          // Field reference delayed (for regulation error calculation)

    ANA_I_MAGNET,               // Current in the magnet
    ANA_I_CIRCUIT,              // Current in the circuit
    ANA_I_RMS,                  // Filtered RMS circuit current for converter protection
    ANA_I_RMS_LOAD,             // Filtered RMS circuit current for load protections
    ANA_I_MEAS,                 // Unfiltered measured current
    ANA_I_MEAS_FLTR,            // Filtered measured current
    ANA_I_MEAS_EXTR,            // Extrapolated measured current
    ANA_I_REF_DELAYED,          // Current reference delayed (for regulation error calculation)

    ANA_V_REF,                  // Voltage reference
    ANA_V_REF_SAT,              // Voltage reference after magnet saturation compensation applied
    ANA_V_REF_LIMITED,          // Voltage reference after saturation compensation and limits applied
    ANA_V_CIRCUIT,              // Voltage across circuit
    ANA_V_MEAS,                 // Measured voltage

    ANA_B_ERR,                  // Field regulation error
    ANA_I_ERR,                  // Current regulation error
    ANA_V_ERR,                  // Voltage regulation error

    ANA_MAX_ABS_B_ERR,          // Max absolute field regulation error
    ANA_MAX_ABS_I_ERR,          // Max absolute current regulation error
    ANA_MAX_ABS_V_ERR,          // Max absolute voltage regulation error

    NUM_ANA_SIGNALS
};

CCLOG_EXT struct cclog_ana_sigs ana_meas_sigs[]     // IMPORTANT: This must be in the same order as enum cclog_ana_meas_idx (above)
#ifdef GLOBALS
= { // Name                  Trailing_step
    { "B_MAGNET",               true  },
    { "B_MEAS",                 false },
    { "B_MEAS_FLTR",            false },
    { "B_MEAS_EXTR",            false },
    { "B_REF_DELAYED",          true  },

    { "I_MAGNET",               true  },
    { "I_CIRCUIT",              true  },
    { "I_RMS",                  false },
    { "I_RMS_LOAD",             false },
    { "I_MEAS",                 false },
    { "I_MEAS_FLTR",            false },
    { "I_MEAS_EXTR",            false },
    { "I_REF_DELAYED",          true  },

    { "V_REF",                  true  },
    { "V_REF_SAT",              true  },
    { "V_REF_LIMITED",          true  },
    { "V_CIRCUIT",              true  },
    { "V_MEAS",                 false },

    { "B_ERR",                  true  },
    { "I_ERR",                  true  },
    { "V_ERR",                  true  },

    { "MAX_ABS_B_ERR",          true  },
    { "MAX_ABS_I_ERR",          true  },
    { "MAX_ABS_V_ERR",          true  },
}
#endif
;

// Measurement rate digital log signals

enum cclog_dig_meas_idx
{
    DIG_B_MEAS_TRIP,            // Field digital signals
    DIG_B_MEAS_LOW,
    DIG_B_MEAS_ZERO,

    DIG_B_REF_CLIP,
    DIG_B_REF_RATE_CLIP,
    DIG_B_REG_ERR_WARN,
    DIG_B_REG_ERR_FLT,

    DIG_I_MEAS_TRIP,            // Current digital signals
    DIG_I_MEAS_LOW,
    DIG_I_MEAS_ZERO,

    DIG_I_RMS_WARN,
    DIG_I_RMS_FLT,
    DIG_I_RMS_LOAD_WARN,
    DIG_I_RMS_LOAD_FLT,

    DIG_I_REF_CLIP,
    DIG_I_REF_RATE_CLIP,
    DIG_I_REG_ERR_WARN,
    DIG_I_REG_ERR_FLT,

    DIG_V_REF_CLIP,             // Voltage digital signals
    DIG_V_REF_RATE_CLIP,
    DIG_V_REG_ERR_WARN,
    DIG_V_REG_ERR_FLT,

    DIG_INVALID_MEAS,

    NUM_DIG_SIGNALS
};

CCLOG_EXT struct cclog_dig_sigs dig_meas_sigs[]     // IMPORTANT: This must be in the same order as enum cclog_dig_meas_idx (above)
#ifdef GLOBALS
= { // Name
    { "B_MEAS_TRIP"     },
    { "B_MEAS_LOW"      },
    { "B_MEAS_ZERO"     },

    { "B_REF_CLIP"      },
    { "B_REF_RATE_CLIP" },
    { "B_REG_ERR_WARN"  },
    { "B_REG_ERR_FLT"   },

    { "I_MEAS_TRIP"     },
    { "I_MEAS_LOW"      },
    { "I_MEAS_ZERO"     },

    { "I_RMS_WARN"      },
    { "I_RMS_FLT"       },
    { "I_RMS_LOAD_WARN" },
    { "I_RMS_LOAD_FLT"  },

    { "I_REF_CLIP"      },
    { "I_REF_RATE_CLIP" },
    { "I_REG_ERR_WARN"  },
    { "I_REG_ERR_FLT"   },

    { "V_REF_CLIP"      },
    { "V_REF_RATE_CLIP" },
    { "V_REG_ERR_WARN"  },
    { "V_REG_ERR_FLT"   },

    { "INVALID_MEAS"    },
}
#endif
;

// Measurement rate log structure

CCLOG_EXT struct cclog meas_log
#ifdef GLOBALS
= {
    ana_meas_sigs,
    NUM_ANA_SIGNALS,
    dig_meas_sigs,
    NUM_DIG_SIGNALS,
}
#endif
;

// Function declarations

void     ccLogInit              (void);
void     ccLogStoreReg          (struct cclog *log, double time);
void     ccLogStoreMeas         (double time);
uint32_t ccLogReportBadValues   (struct cclog *log);

#endif
// EOF
