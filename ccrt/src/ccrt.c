/*!
 * @file  ccrt/scr/ccRt.c
 *
 * @brief ccrt main function
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

/*
 * Create NVS system based on files for converter types
 * Add prompt line with status information
 * Add real-time thread
 * NVS to files
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <libgen.h>
#include <errno.h>
#include <pthread.h>
#include <signal.h>
#include <time.h>

// Declare all variables in ccTest.c

#define GLOBALS

// Include cctest program header files

#include "ccCmds.h"
#include "ccRt.h"
#include "ccParse.h"
#include "ccFile.h"
#include "ccInit.h"
#include "ccRun.h"
#include "ccSim.h"
#include "ccRef.h"
#include "ccLog.h"
#include "ccFlot.h"

// Constants

#define TIMER_PRIORITY  0        // 0 for non-realtime Linux kernel

// Static variables

static char             cwd_buf[CC_PATH_LEN];

struct cc_thread
{
    pthread_attr_t          thread_attr;    // Thread attricute
    struct sched_param      thread_param;   // Thread scheduling parameters
    struct itimerspec       timer_spec;     // Timer specification
    timer_t                 timer_id;       // ID of new timer
    struct sigevent         event;          // Event configuration
};

static struct cc_thread      rt_thread;
static struct cc_thread      sc_thread;



static void StartRtThread(uint32_t rt_period_s, uint32_t rt_period_ns, struct cc_thread *cc_thread, void (*function)(union sigval))
{
    // Configure thread attributes

    if(pthread_attr_init(&cc_thread->thread_attr) == -1)
    {
        printf("Error - pthread_attr_init\n");
        exit(EXIT_FAILURE);
    }

#if TIMER_PRIORITY == 0
    pthread_attr_setinheritsched(&cc_thread->thread_attr, PTHREAD_INHERIT_SCHED);

    sched_getparam(getpid(), &cc_thread->thread_param);
    pthread_attr_setschedparam(&cc_thread->thread_attr, &cc_thread->thread_param);
#else
    pthread_attr_setinheritsched(&cc_thread->thread_attr, PTHREAD_EXPLICIT_SCHED);
    pthread_attr_setschedpolicy( &cc_thread->thread_attr, SCHED_FIFO);

    sched_getparam(getpid(), &cc_thread->thread_param);
    cc_thread->thread_param.sched_priority = TIMER_PRIORITY;
    pthread_attr_setschedparam(&cc_thread->thread_attr, &cc_thread->thread_param);
#endif

    // Configure event handler

    cc_thread->event.sigev_notify              = SIGEV_THREAD;
    cc_thread->event.sigev_notify_attributes   = &cc_thread->thread_attr;
    cc_thread->event.sigev_notify_function     = function;

    // Create timer

    if(timer_create(CLOCK_REALTIME, &cc_thread->event, &cc_thread->timer_id))
    {
        printf("Error - timer_create : %s (%d)\n", strerror(errno), errno);
        exit(EXIT_FAILURE);
    }

    // Configure timer specification

    cc_thread->timer_spec.it_value.tv_sec     = 1;
    cc_thread->timer_spec.it_value.tv_nsec    = rt_period_ns;
    cc_thread->timer_spec.it_interval.tv_sec  = rt_period_s;
    cc_thread->timer_spec.it_interval.tv_nsec = rt_period_ns;

    // Start the timer

    if(timer_settime(cc_thread->timer_id, 0, &cc_thread->timer_spec, NULL))
    {
        printf("Error - timer_settime : %s (%d)\n", strerror(errno), errno);
        exit(EXIT_FAILURE);
    }
}



static void AtExit(void)
{
    // Restore current working directory from when the program started

    chdir(cwd_buf);
}



int main(int argc, char **argv)
{
    uint32_t rt_period_ns = CC_ITER_PERIOD_US * 1000;     // 1ms by default
    char     line[CC_PATH_LEN];
    char    *script_file = "";
    char    *default_converter = "default";

    // Usage: ccrt [converter_name [script]]

    if(argc > 3)
    {
        printf("usage: %s [converter_name [command]]\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    else if(argc == 1)
    {
        ccfile.converter = default_converter;
    }
    else
    {
        ccfile.converter = argv[1];

        if(argc == 3)
        {
            rt_period_ns /= CC_OFFLINE_ACCELERATION;     // Run faster than real-time when processing a script file
            script_file = argv[2];
        }
    }

    snprintf(line, CC_PATH_LEN, "read %s", script_file);

    // Save current working so that it can be restored at exit

    if(getcwd(cwd_buf, sizeof(cwd_buf)) != NULL)
    {
        if(atexit(AtExit))
        {
            puts("Warning - atexit() failed");
        }
    }
    else
    {
        printf("Warning - failed to get current directory : %s (%d)\n", strerror(errno), errno);
    }

    // Try to set path to ccrt/converters/{converter}

    if(ccFileCwd(dirname(argv[0]))   == EXIT_FAILURE ||
       ccFileCwd("../../converters") == EXIT_FAILURE)
    {
        exit(EXIT_FAILURE);
    }

    if(chdir(ccfile.converter) != EXIT_SUCCESS)
    {
        printf("Error - unknown converter: %s\n", ccParseAbbreviateArg(ccfile.converter));
        exit(EXIT_FAILURE);
    }

    // Welcome message

    printf("\nWelcome to ccrt v%.2f - Converter type: %s\n\n", CC_VERSION, ccfile.converter);

    // Check the necessary sub-directories exist, or try to make them if they don't

    if(ccFileMakePath("config")  == EXIT_FAILURE ||
       ccFileMakePath("ref")     == EXIT_FAILURE ||
       ccFileMakePath("scripts") == EXIT_FAILURE ||
       ccFileMakePath("logs")    == EXIT_FAILURE)
    {
        exit(EXIT_FAILURE);
    }

    // Initialise ccrt parameters

    ccInitPars();

    // Read in all the config and reference parameters

    if(ccFileReadAll("config") == EXIT_FAILURE ||
       ccFileReadAll("ref")    == EXIT_FAILURE)
    {
        exit(EXIT_FAILURE);
    }

    // Change directory to be scripts

    if(ccFileCwd("scripts") == EXIT_FAILURE)
    {
        exit(EXIT_FAILURE);
    }

    // Rewrite all configuration files

    if(ccFileSaveAllConfigPars() == EXIT_FAILURE)
    {
        exit(EXIT_FAILURE);
    }

    // Try to arm the reference for all cycle selectors

    puts("Arming functions");

    ccCmdsArm(0,NULL);

    // Initialise time to start on a second boundary

    gettimeofday(&ccrun.iter_time, NULL);

    ccrun.iter_time.tv_usec = 0;

    // Create real-time thread for simulation, running on a timer with specified period

    puts("Starting real-time thread");

    StartRtThread(0, rt_period_ns, &rt_thread, ccRun);

    // Create real-time thread for simulation, running on a timer with specified period

    puts("Starting supercycle thread");

    StartRtThread(1, 0, &sc_thread, ccSimSuperCycle);

    // Initialise simulation

    puts("Starting simulation");

    regMgrSimInit(&reg_mgr, REG_NONE, 0.0);

    // Read from stdin or from the named script

    exit(ccParseLine(line));
}

// EOF
