/*!
 * @file  ccrt/inc/pars/ref.h
 *
 * @brief ccrt REF parameters
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

#ifndef CCPARS_REF_H
#define CCPARS_REF_H

#include "ccRt.h"
#include "ccPars.h"

// GLOBALS should be defined in the source file where global variables should be defined

#ifdef GLOBALS
#define CCPARS_REF_EXT
#else
#define CCPARS_REF_EXT extern
#endif

// Function type enum - must match order of struct fgfunc funcs[] in ccRef.h

enum fg_types
{
    FG_NONE,
    FG_PLEP,
    FG_RAMP,
    FG_PPPL,
    FG_TABLE,
    FG_STEPS,
    FG_SQUARE,
    FG_SINE,
    FG_COSINE,
    FG_LTRIM,
    FG_CTRIM,
    FG_PULSE,
};

CCPARS_REF_EXT struct ccpars_enum enum_function_type[]
#ifdef GLOBALS
= { //                              flags
    { FG_NONE,        "NONE",       0                  },
    { FG_PLEP,        "PLEP",       CC_ENUM_GREEN      },
    { FG_RAMP,        "RAMP",       CC_ENUM_GREEN      },
    { FG_PPPL,        "PPPL",       CC_ENUM_GREEN      },
    { FG_TABLE,       "TABLE",      CC_ENUM_GREEN      },
    { FG_STEPS,       "STEPS",      CC_ENUM_CYAN       },
    { FG_SQUARE,      "SQUARE",     CC_ENUM_CYAN       },
    { FG_SINE,        "SINE",       CC_ENUM_CYAN       },
    { FG_COSINE,      "COSINE",     CC_ENUM_CYAN       },
    { FG_LTRIM,       "LTRIM",      CC_ENUM_GREEN      },
    { FG_CTRIM,       "CTRIM",      CC_ENUM_GREEN      },
    { FG_PULSE,       "PULSE",      CC_ENUM_GREEN      },
    { 0,               NULL                            },
}
#endif
;

CCPARS_REF_EXT struct ccpars_enum enum_reg_mode[]
#ifdef GLOBALS
= { //                              flags
    { REG_NONE,       "NONE",       CC_ENUM_READ_ONLY  },
    { REG_VOLTAGE,    "VOLTAGE",    0                  },
    { REG_CURRENT,    "CURRENT",    0                  },
    { REG_FIELD,      "FIELD",      0                  },
    { 0,               NULL                            },
}
#endif
;

CCPARS_REF_EXT struct ccpars_enum enum_fg_error[]
#ifdef GLOBALS
= {
    { FG_OK,                         "OK"                         },
    { FG_BAD_ARRAY_LEN,              "BAD_ARRAY_LEN"              },
    { FG_BAD_PARAMETER,              "BAD_PARAMETER"              },
    { FG_INVALID_TIME,               "INVALID_TIME"               },
    { FG_OUT_OF_LIMITS,              "OUT_OF_LIMITS"              },
    { FG_OUT_OF_RATE_LIMITS,         "OUT_OF_RATE_LIMITS"         },
    { FG_OUT_OF_ACCELERATION_LIMITS, "OUT_OF_ACCELERATION_LIMITS" },
    { 0,                              NULL                        },
}
#endif
;

CCPARS_REF_EXT struct ccpars_enum enum_func_pol[]
#ifdef GLOBALS
= {
    { FG_FUNC_POL_ZERO,      "ZERO"     },
    { FG_FUNC_POL_POSITIVE,  "POSITIVE" },
    { FG_FUNC_POL_NEGATIVE,  "NEGATIVE" },
    { FG_FUNC_POL_BOTH,      "BOTH"     },
    { 0,                      NULL      },
}
#endif
;

// Ref parameters structure

struct ccpars_ref
{   // Read/write
    enum reg_enabled_disabled   play;                   // Play reference (ENABLED, DISABLED)
    enum reg_mode               reg_mode;               // Regulation mode (VOLTAGE, CURRENT or FIELD)
    enum fg_types               function;               // Ref function type
    // Read-only
    enum reg_mode               armed_reg_mode;         // Armed regulation mode (VOLTAGE, CURRENT or FIELD)
    enum fg_types               armed_function;         // Armed reference function type
    struct fg_meta              fg_meta;                // Reference function meta data
};

CCPARS_REF_EXT struct ccpars_ref ccpars_ref[CC_NUM_CYC_SELS]
#ifdef GLOBALS
= {//   Default value                  Parameter
  {   REG_ENABLED,              // REF PLAY(0)
      REG_VOLTAGE,              // REF REG_MODE(0)
      FG_SINE        }          // REF FUNCTION(0)
}
#endif
;

// Ref parameters enum to allow access to named fields

enum ref_pars_index_enum
{
    REF_PLAY,
    REF_REG_MODE,
    REF_FUNCTION,
};

CCPARS_REF_EXT struct ccpars ref_pars[]
#ifdef GLOBALS
= {// "Signal name"      type,     max_n_els,          *enum,                         *value,                     num_defaults,                  cyc_sel_step,    flags
    { "PLAY",            PAR_ENUM,     1,               enum_enabled_disabled, { .u = &ccpars_ref[0].play                    }, 1,               sizeof(struct ccpars_ref), PARS_RW|PARS_CFG },
    { "REG_MODE",        PAR_ENUM,     1,               enum_reg_mode,         { .u = &ccpars_ref[0].reg_mode                }, 1,               sizeof(struct ccpars_ref), PARS_RW|PARS_REF },
    { "FUNCTION",        PAR_ENUM,     1,               enum_function_type,    { .u = &ccpars_ref[0].function                }, 1,               sizeof(struct ccpars_ref), PARS_RW|PARS_REF },
    { "ARMED_REG_MODE",  PAR_ENUM,     1,               enum_reg_mode,         { .u = &ccpars_ref[0].armed_reg_mode          }, 1,               sizeof(struct ccpars_ref), PARS_RO          },
    { "ARMED_FUNCTION",  PAR_ENUM,     1,               enum_function_type,    { .u = &ccpars_ref[0].armed_function          }, 1,               sizeof(struct ccpars_ref), PARS_RO          },
    { "STATUS",          PAR_ENUM,     1,               enum_fg_error,         { .u = &ccpars_ref[0].fg_meta.fg_error        }, 1,               sizeof(struct ccpars_ref), PARS_RO          },
    { "ERR_INDEX",       PAR_UNSIGNED, 1,               NULL,                  { .u = &ccpars_ref[0].fg_meta.error.index     }, 1,               sizeof(struct ccpars_ref), PARS_RO          },
    { "ERR_DATA",        PAR_FLOAT,    FG_ERR_DATA_LEN, enum_fg_error,         { .f =  ccpars_ref[0].fg_meta.error.data      }, FG_ERR_DATA_LEN, sizeof(struct ccpars_ref), PARS_RO          },
    { "POLARITY",        PAR_ENUM,     1,               enum_func_pol,         { .u = &ccpars_ref[0].fg_meta.polarity        }, 1,               sizeof(struct ccpars_ref), PARS_RO          },
    { "LIMITS_INVERTED", PAR_BOOL,     1,               NULL,                  { .b = &ccpars_ref[0].fg_meta.limits_inverted }, 1,               sizeof(struct ccpars_ref), PARS_RO          },
    { "DELAY",           PAR_DOUBLE,   1,               NULL,                  { .d = &ccpars_ref[0].fg_meta.delay           }, 1,               sizeof(struct ccpars_ref), PARS_RO          },
    { "DURATION",        PAR_FLOAT,    1,               NULL,                  { .f = &ccpars_ref[0].fg_meta.duration        }, 1,               sizeof(struct ccpars_ref), PARS_RO          },
    { "START",           PAR_FLOAT,    1,               NULL,                  { .f = &ccpars_ref[0].fg_meta.range.start     }, 1,               sizeof(struct ccpars_ref), PARS_RO          },
    { "END",             PAR_FLOAT,    1,               NULL,                  { .f = &ccpars_ref[0].fg_meta.range.end       }, 1,               sizeof(struct ccpars_ref), PARS_RO          },
    { "MIN",             PAR_FLOAT,    1,               NULL,                  { .f = &ccpars_ref[0].fg_meta.range.min       }, 1,               sizeof(struct ccpars_ref), PARS_RO          },
    { "MAX",             PAR_FLOAT,    1,               NULL,                  { .f = &ccpars_ref[0].fg_meta.range.max       }, 1,               sizeof(struct ccpars_ref), PARS_RO          },
    { NULL }
}
#endif
;

#endif
// EOF

