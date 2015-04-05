/*!
 * @file  ccrt/inc/ccPars.h
 *
 * @brief ccrt header file for ccPars.c
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

#ifndef CCPARS_H
#define CCPARS_H

#include <stdint.h>
#include <libreg.h>
#include <stdbool.h>

// GLOBALS should be defined in the source file where global variables should be defined

#ifdef GLOBALS
#define CCPARS_EXT
#else
#define CCPARS_EXT extern
#endif

// Constants

#define PARS_INDENT                 34
#define PARS_MAX_PRINT_LINE_LEN     (CC_MAX_FILE_LINE_LEN*8)      // Allow for longest print line for table
#define PARS_INT_FORMAT             "% d "
#define PARS_FLOAT_FORMAT           "% .6E "
#define PARS_TIME_FORMAT            "% .6f "
#define PARS_STRING_FORMAT          " %s "

// struct ccpars flags

#define PARS_FIXLEN                 0x0001        // Fixed length parameter
#define PARS_RO                     0x0002        // Read-only parameter
#define PARS_RW                     0x0004        // Read/write parameter
#define PARS_CFG                    0x0008        // Config parameter
#define PARS_REF                    0x0010        // Reference parameter
#define PARS_REG                    0x0020        // Libreg parameter

// struct ccpars_enum flags

#define CC_ENUM_READ_ONLY           0x0001
#define CC_ENUM_RED                 0x0002
#define CC_ENUM_YELLOW              0x0004
#define CC_ENUM_GREEN               0x0008
#define CC_ENUM_CYAN                0x0010

// Enums

enum CCpars_type
{
    PAR_UNSIGNED,
    PAR_FLOAT,
    PAR_DOUBLE,
    PAR_STRING,
    PAR_ENUM,
    PAR_BOOL
};

// Size of type array

CCPARS_EXT uint32_t ccpars_sizeof_type[]
#ifdef GLOBALS
= {
    sizeof(uint32_t),
    sizeof(float),
    sizeof(double),
    sizeof(char *),
    sizeof(uint32_t),
    sizeof(bool)
}
#endif
;

// Structures

struct CCpars
{
    char               *name;
    enum CCpars_type    type;
    uint32_t            max_num_elements;
    struct CCpars_enum *ccpars_enum;
    union CCvalue_p
    {
        char           *c;
        uint32_t       *u;
        bool           *b;
        float          *f;
        double         *d;
        char          **s;
    } value_p;

    uint32_t            num_default_elements;
    uint32_t            cyc_sel_step;
    uint32_t            flags;
    uint32_t           *num_elements;           // Initialised by ccInitParNumElements()
};

struct CCpars_enum
{
    uint32_t            value;
    char *              string;
    uint32_t            flags;
};

// ENABLED/DISABLED enum

CCPARS_EXT struct CCpars_enum enum_enabled_disabled[]
#ifdef GLOBALS
= {
    { REG_DISABLED,     "DISABLED"      },
    { REG_ENABLED,      "ENABLED"       },
    { 0,                NULL            },
}
#endif
;

// Function declarations

uint32_t ccParsGet                  (char *cmd_name, struct CCpars *par, char *remaining_line);
int32_t  ccParsEnumIndex            (struct CCpars_enum *par_enum, uint32_t value);
char    *ccParsEnumString           (struct CCpars_enum *par_enum, uint32_t value);
void     ccParsPrint                (FILE *f, char *cmd_name, struct CCpars *par, uint32_t cyc_sel, uint32_t array_idx);
void     ccParsPrintAll             (FILE *f, char *cmd_name, struct CCpars *par, uint32_t cyc_sel, uint32_t array_idx, uint32_t flags);
void     ccParsPrintError           (const char * format, ...);

#endif
// EOF
