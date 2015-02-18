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

#include "ccRun.h"
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
    float                      *buf;                    // Signal buffer (for FLOT output)
    float                       time_offset;            // Time offset for trace
    float                       value;                  // Signal value
    uint32_t                    num_bad_values;         // Counter for bad values
    bool                        is_enabled;             // Signal in use flag
};

struct cclog_dig_sigs
{
    char                       *name;                   // Signal name
    bool const                 *source;                 // Pointer to source of digital signal
    uint8_t                    *buf;                    // Signal buffer (for FLOT output)
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
    { "B_MEAS_REG",             true,       &regConvVar(conv, BREG_MEAS               )     },
    { "B_REF",                  true,       &regConvVar(conv, BREG_REF                )     },
    { "B_REF_LIMITED",          true,       &regConvVar(conv, BREG_REF_LIMITED        )     },
    { "B_REF_RST",              true,       &regConvVar(conv, BREG_REF_RST            )     },
    { "B_REF_OPENLOOP",         true,       &regConvVar(conv, BREG_REF_OPENLOOP       )     },
    { "B_TRACK_DELAY",          true,       &regConvVar(conv, BREG_TRACK_DELAY_PERIODS)     },
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

// RMS currents

// Global variables for RMS logging signals

CCLOG_EXT float    i_rms;
CCLOG_EXT float    i_rms_load;

// Current regulation log

CCLOG_EXT struct cclog_ana_sigs ana_ireg_sigs[]
#ifdef GLOBALS
= { // Name                  Trailing_step
    { "I_MEAS_REG",             true,       &regConvVar(conv, IREG_MEAS               )     },
    { "I_REF",                  true,       &regConvVar(conv, IREG_REF                )     },
    { "I_REF_LIMITED",          true,       &regConvVar(conv, IREG_REF_LIMITED        )     },
    { "I_REF_RST",              true,       &regConvVar(conv, IREG_REF_RST            )     },
    { "I_REF_OPENLOOP",         true,       &regConvVar(conv, IREG_REF_OPENLOOP       )     },
    { "I_TRACK_DELAY",          true,       &regConvVar(conv, IREG_TRACK_DELAY_PERIODS)     },
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
    { "B_MAGNET",               true ,           &regConvVar(conv, SIM_B_MAGNET)           },
    { "B_MEAS",                 false,           &regConvVar(conv, MEAS_B_UNFILTERED)      },
    { "B_MEAS_FLTR",            false,           &regConvVar(conv, MEAS_B_FILTERED)        },
    { "B_MEAS_EXTR",            false,           &regConvVar(conv, MEAS_B_EXTRAPOLATED)    },
    { "B_REF_DELAYED",          true ,           &regConvVar(conv, BREG_REF_DELAYED)       },

    { "I_MAGNET",               true ,           &regConvVar(conv, SIM_I_MAGNET)           },
    { "I_CIRCUIT",              true ,           &regConvVar(conv, SIM_I_CIRCUIT)          },
    { "I_RMS",                  false,           &i_rms                                    },
    { "I_RMS_LOAD",             false,           &i_rms_load                               },
    { "I_MEAS",                 false,           &regConvVar(conv, MEAS_I_UNFILTERED)      },
    { "I_MEAS_FLTR",            false,           &regConvVar(conv, MEAS_I_FILTERED)        },
    { "I_MEAS_EXTR",            false,           &regConvVar(conv, MEAS_I_EXTRAPOLATED)    },
    { "I_REF_DELAYED",          true ,           &regConvVar(conv, IREG_REF_DELAYED)       },

    { "V_REF",                  true ,           &regConvVar(conv, V_REF)                  },
    { "V_REF_SAT",              true ,           &regConvVar(conv, V_REF_SAT)              },
    { "V_REF_LIMITED",          true ,           &regConvVar(conv, V_REF_LIMITED)          },
    { "V_CIRCUIT",              true ,           &regConvVar(conv, SIM_V_CIRCUIT)          },
    { "V_MEAS",                 false,           &regConvVar(conv, V_MEAS)                 },

    { "B_ERR",                  true ,           &regConvVar(conv, BREG_ERR)               },
    { "I_ERR",                  true ,           &regConvVar(conv, IREG_ERR)               },
    { "V_ERR",                  true ,           &regConvVar(conv, V_ERR)                  },

    { "MAX_ABS_B_ERR",          true ,           &regConvVar(conv, BREG_MAX_ABS_ERR)       },
    { "MAX_ABS_I_ERR",          true ,           &regConvVar(conv, IREG_MAX_ABS_ERR)       },
    { "MAX_ABS_V_ERR",          true ,           &regConvVar(conv, V_MAX_ABS_ERR)          },
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
    { "B_MEAS_TRIP"     ,        &regConvVar(conv,FLAG_B_MEAS_TRIP)           },
    { "B_MEAS_LOW"      ,        &regConvVar(conv,FLAG_B_MEAS_LOW)            },
    { "B_MEAS_ZERO"     ,        &regConvVar(conv,FLAG_B_MEAS_ZERO)           },

    { "B_REF_CLIP"      ,        &regConvVar(conv,FLAG_B_REF_CLIPPED)         },
    { "B_REF_RATE_CLIP" ,        &regConvVar(conv,FLAG_B_REF_RATE_CLIPPED)    },
    { "B_REG_ERR_WARN"  ,        &regConvVar(conv,FLAG_B_REG_ERR_WARNING)     },
    { "B_REG_ERR_FLT"   ,        &regConvVar(conv,FLAG_B_REG_ERR_FAULT)       },

    { "I_MEAS_TRIP"     ,        &regConvVar(conv,FLAG_I_MEAS_TRIP)           },
    { "I_MEAS_LOW"      ,        &regConvVar(conv,FLAG_I_MEAS_LOW)            },
    { "I_MEAS_ZERO"     ,        &regConvVar(conv,FLAG_I_MEAS_ZERO)           },

    { "I_RMS_WARN"      ,        &regConvVar(conv,FLAG_I_RMS_WARNING)         },
    { "I_RMS_FLT"       ,        &regConvVar(conv,FLAG_I_RMS_FAULT)           },
    { "I_RMS_LOAD_WARN" ,        &regConvVar(conv,FLAG_I_RMS_LOAD_WARNING)    },
    { "I_RMS_LOAD_FLT"  ,        &regConvVar(conv,FLAG_I_RMS_LOAD_FAULT)      },

    { "I_REF_CLIP"      ,        &regConvVar(conv,FLAG_I_REF_CLIPPED)         },
    { "I_REF_RATE_CLIP" ,        &regConvVar(conv,FLAG_I_REF_RATE_CLIPPED)    },
    { "I_REG_ERR_WARN"  ,        &regConvVar(conv,FLAG_I_REG_ERR_WARNING)     },
    { "I_REG_ERR_FLT"   ,        &regConvVar(conv,FLAG_I_REG_ERR_FAULT)       },

    { "V_REF_CLIP"      ,        &regConvVar(conv,FLAG_V_REF_CLIPPED)         },
    { "V_REF_RATE_CLIP" ,        &regConvVar(conv,FLAG_V_REF_RATE_CLIPPED)    },
    { "V_REG_ERR_WARN"  ,        &regConvVar(conv,FLAG_V_REG_ERR_WARNING)     },
    { "V_REG_ERR_FLT"   ,        &regConvVar(conv,FLAG_V_REG_ERR_FAULT)       },

    { "INVALID_MEAS"    ,        &ccrun.invalid_meas.flag                     },
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
