/*!
 * @file  ccrt/scr/ccStatus.c
 *
 * @brief ccrt status reporting functions
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
#include <stdarg.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <float.h>
#include <time.h>
#include <errno.h>

// Include libterm header file to get terminal control sequences

#include "libterm.h"

// Include ccrt program header files

#include "ccCmds.h"
#include "ccRt.h"
#include "ccParse.h"
#include "ccFile.h"
#include "ccRef.h"
#include "ccRun.h"



static void ccStatusEnumPointer(struct ccpars_enum *enum_p, char *fmt, char *format)
{
    if(enum_p->flags & CC_ENUM_RED)
    {
        printf(TERM_CSI TERM_FG_RED "%s" TERM_SGR, format);
    }
    else if(enum_p->flags & CC_ENUM_YELLOW)
    {
        printf(TERM_CSI TERM_FG_YELLOW "%s" TERM_SGR, format);
    }
    else if(enum_p->flags & CC_ENUM_GREEN)
    {
        printf(TERM_CSI TERM_FG_GREEN "%s" TERM_SGR, format);
    }
    else if(enum_p->flags & CC_ENUM_CYAN)
    {
        printf(TERM_CSI TERM_FG_CYAN "%s" TERM_SGR, format);
    }
    else if(format[0] != '\0')
    {
        printf(TERM_CSI "%s" TERM_SGR, format);
    }

    printf(fmt, enum_p->string);

    printf(TERM_NORMAL);
}



static void ccStatusEnumValue(struct ccpars *par, char *fmt)
{
    int32_t  enum_idx = ccParsEnumIndex(par->ccpars_enum, *par->value_p.u);
    struct ccpars_enum *enum_p = par->ccpars_enum + enum_idx;

    if(enum_idx < 0)
    {
        printf(fmt, "?");
    }
    else
    {
        ccStatusEnumPointer(enum_p, fmt, "");
    }
}


static void ccStatusMeas(char *label, enum reg_mode reg_mode, float meas)
{
    printf("  " TERM_CSI TERM_BOLD TERM_SGR "%s" TERM_NORMAL ":", label);

    if(reg_mgr.reg_mode == reg_mode)
    {
        if(reg_mgr.is_openloop)
        {
            printf(TERM_CSI TERM_FG_CYAN TERM_SGR);
        }
        else
        {
            printf(TERM_CSI TERM_FG_GREEN TERM_SGR);
        }
    }

    printf("%10.3f" TERM_NORMAL, meas);
}



static void ccStatusFlags(struct cccmds *cmd, char *active_fg, char *active_bg)
{
    struct ccpars *par = cmd->pars;

    printf("  " TERM_CSI TERM_BOLD TERM_SGR "%s" TERM_NORMAL ":", cmd->name);

    while(par->name != NULL)
    {
        if(par->type == PAR_ENUM && par->ccpars_enum == enum_enabled_disabled)
        {
            if(*par->value_p.u == REG_ENABLED)
            {
                printf(TERM_CSI "%s%s" TERM_SGR, active_fg, active_bg);
            }

            // Print first letter of the parameter name and cancel formatting

            printf("%c%s", par->name[0], TERM_NORMAL);
        }

        par++;
    }
}



void ccStatus(bool overwrite)
{
    struct timeval tp              = ccrun.iter_time;
    double         time_till_event = ccrun.time_till_event_us * 1.0E-6;
    uint32_t       cyc_idx;
    uint32_t       cyc_sel;
    uint32_t       ref_cyc_sel;

    struct tm      tm;

    localtime_r(&tp.tv_sec, &tm);

    // If overwrite is true then move up to overwrite previous status

    if(overwrite)
    {
        printf(TERM_CSI  "3" TERM_UP "\r");
    }
    else
    {
        putchar('\n');
    }

    // ------------ Display Line 1 of status ---------------

    // Time of day

    printf(TERM_CSI TERM_BOLD TERM_SGR "%02u:%02u:%02u.%03u  " TERM_NORMAL,
            tm.tm_hour, tm.tm_min, tm.tm_sec, (uint32_t)tp.tv_usec/1000);

    // PC state

    printf(TERM_CSI TERM_BOLD TERM_SGR "PC" TERM_NORMAL ":");
    ccStatusEnumValue(&pc_pars   [PC_STATE], "%3s");
    putchar('/');
    ccStatusEnumValue(&state_pars[STATE_PC], "%-5s");

    // Actuation

    printf("  " TERM_CSI TERM_BOLD TERM_SGR "ACT" TERM_NORMAL ":");
    ccStatusEnumValue(&pc_pars   [PC_ACTUATION], "%7s");

    // REF state

    printf("  " TERM_CSI TERM_BOLD TERM_SGR "REF" TERM_NORMAL ":");
    ccStatusEnumValue(&pc_pars   [PC_REF], "%10s");
    putchar('/');
    ccStatusEnumValue(&state_pars[STATE_REF], "%-15s");

    // POLSWITCH state

    if(ccpars_state.polswitch != POLSWITCH_NONE)
    {
        printf("  " TERM_CSI TERM_BOLD TERM_SGR "POLSWITCH" TERM_NORMAL ":");
        ccStatusEnumValue(&polswitch_pars[POLSWITCH_COMMAND], "%9s");
        putchar('/');
        ccStatusEnumValue(&state_pars[STATE_POLSWITCH], "%-9s");
    }

    putchar('\n');

    // ------------ Display Line 2 of status ---------------

    // CYC_IDX/CYC_SEL/REF_CYC_SEL

    pthread_mutex_lock(&ccrun.supercycle.mutex);

    cyc_idx     = ccrun.supercycle.cyc_idx;
    cyc_sel     = ccrun.supercycle.cyc_sel;
    ref_cyc_sel = ccrun.supercycle.ref_cyc_sel;

    pthread_mutex_unlock(&ccrun.supercycle.mutex);

    printf(TERM_CSI TERM_BOLD TERM_SGR "CYC" TERM_NORMAL ":");
    printf("%02u/",cyc_idx);

    if(cyc_sel > 0 && cyc_sel == ccpars_global.test_cyc_sel)
    {
        printf(TERM_CSI TERM_FG_YELLOW TERM_SGR "%02u/", cyc_sel);

        if(cyc_sel == ref_cyc_sel)
        {
            printf(TERM_CSI TERM_FG_GREEN TERM_SGR);
        }

        printf("%02u  " TERM_NORMAL, ref_cyc_sel);
    }
    else
    {
        printf("%02u/%02u  ",cyc_sel, ref_cyc_sel);
    }

    // Time till event

    printf(TERM_CSI TERM_BOLD TERM_SGR "EVT" TERM_NORMAL ":");

    if(time_till_event < 0)
    {
        printf(TERM_CSI TERM_FG_YELLOW TERM_SGR "%+08.3f", time_till_event);
    }
    else if(time_till_event < (double)(ccpars_global.cycle_selector[cyc_idx] % 100))
    {
        printf(TERM_CSI TERM_FG_GREEN TERM_SGR "%+08.3f", time_till_event);
    }
    else
    {
        printf(TERM_CSI TERM_FG_RED TERM_SGR "%+08.3f", time_till_event);
    }

    // Function type

    printf("  " TERM_CSI TERM_BOLD TERM_SGR "FUNC" TERM_NORMAL ":");
    ccStatusEnumPointer(&enum_function_type[ccrun.function], "%-5s",
            (ccrun.function != FG_NONE && ccrun.play == false) ? TERM_REVERSE : "");

    putchar(' ');

    // Field measurement

    ccStatusMeas("B", REG_FIELD,   reg_mgr.b.meas.signal[REG_MEAS_EXTRAPOLATED]);
    ccStatusMeas("I", REG_CURRENT, reg_mgr.i.meas.signal[REG_MEAS_EXTRAPOLATED]);
    ccStatusMeas("V", REG_VOLTAGE, reg_mgr.v.meas);

    ccStatusFlags(&cmds[CMD_FAULTS],   TERM_FG_WHITE, TERM_BG_RED);
    ccStatusFlags(&cmds[CMD_WARNINGS], TERM_FG_BLACK, TERM_BG_YELLOW);

    // Scroll window to anticipate newline when user presses enter

    printf("\n\n" TERM_CSI "2" TERM_UP);

    // Display prompt

    printf("\n\r" CC_PROMPT);
}

// EOF
