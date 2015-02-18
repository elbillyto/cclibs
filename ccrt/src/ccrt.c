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

// Declare all variables in ccTest.c

#define GLOBALS

// Include cctest program header files

#include "ccCmds.h"
#include "ccRt.h"
#include "ccParse.h"
#include "ccFile.h"
#include "ccInit.h"
#include "ccRun.h"
#include "ccRef.h"
#include "ccLog.h"
#include "ccFlot.h"

// Static variables

static char cwd_buf[CC_PATH_LEN];



void AtExit(void)
{
    // Restore current working directory from when the program started

    chdir(cwd_buf);
}



int main(int argc, char **argv)
{
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

    printf("\nWelcome to ccrt v%.2f - Converter type: %s\n", CC_VERSION, ccfile.converter);

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

    // Initialise simulation

    regConvSimInit(&conv, ccpars_ref[ccpars_global.cycle_selector[0]].reg_mode, 0);

    // Try to arm the reference for all cycle selectors

    ccCmdsArm(0,NULL);

    // Create real-time thread for simulation


    gettimeofday(&ccrun.iter_time, NULL);


    // Read from stdin or from the named script

    exit(ccParseLine(line));
}

// EOF
