/*---------------------------------------------------------------------------------------------------------*\
  File:     ccTest.c                                                                    Copyright CERN 2015

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

  Purpose:  Test program for libfg and libreg (function generation and regulation)
\*---------------------------------------------------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>
#include <errno.h>

// Declare all variables in ccTest.c

#define GLOBALS

// Include cctest program header files

#include "ccCmds.h"
#include "ccTest.h"
#include "ccParse.h"
#include "ccFile.h"
#include "ccInit.h"
#include "ccRun.h"
#include "ccRef.h"
#include "ccLog.h"
#include "ccFlot.h"

// Default commands to run on start-up

char *default_commands[] =
{
    "GLOBAL GROUP   sandbox",
    "GLOBAL PROJECT FG",
    "GLOBAL FILE    cctest",
    NULL
};

/*---------------------------------------------------------------------------------------------------------*/
int main(int argc, char **argv)
/*---------------------------------------------------------------------------------------------------------*/
{
    uint32_t exit_status = EXIT_SUCCESS;
    char   **cmd;
    char     line[40];

    printf("\nWelcome to cctest v%.2f\n", CC_VERSION);

    // Get path to cctest project root

    ccFileGetBasePath(argv[0]);

    // Initialise parameter num_elements arrays

    ccInitPars();

    // Set default group, project and filename

    for(cmd = default_commands ; *cmd != NULL ; cmd++)
    {
        if(ccParseLine(strcpy(line,*cmd)) == EXIT_FAILURE)
        {
            exit(EXIT_FAILURE);
        }
    }

    // If no arguments supplied, read from stdin

    if(argc == 1)
    {
        ccfile.input[0].line_number++;

        exit_status = ccParseLine("read");
    }
    else
    {
        // else process all arguments unless an error is reported

        while(exit_status == EXIT_SUCCESS && --argc > 0)
        {
            ccfile.input[0].line_number++;

            exit_status = ccParseLine(*(++argv));
        }
    }

    // Report exit status

    exit(exit_status);
}

// EOF
