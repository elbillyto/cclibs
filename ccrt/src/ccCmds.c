/*!
 * @file  ccrt/scr/ccCmds.c
 *
 * @brief ccrt command processing functions
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
#include <errno.h>

// Include cctest program header files

#include "ccCmds.h"
#include "ccRt.h"
#include "ccParse.h"
#include "ccFile.h"
#include "ccLog.h"
#include "ccFlot.h"
#include "ccInit.h"
#include "ccRun.h"
#include "ccStatus.h"




uint32_t ccCmdsPar(uint32_t cmd_idx, char *remaining_line)
{
    // If no parameter name provided then print all parameter values

    if(remaining_line == NULL)
    {
        ccParsPrintAll(stdout, cmds[cmd_idx].name, cmds[cmd_idx].pars, ccfile.cyc_sel, ccfile.array_idx, PARS_RW|PARS_RO);
    }
    else // else parameter name argument provided or '+' or '-'
    {
        struct CCpars *par_matched;

        // '+' for read/write parameters only, '-' for read-only parameters only

        if(remaining_line[0] == '+' && remaining_line[1] == '\n')
        {
            ccParsPrintAll(stdout, cmds[cmd_idx].name, cmds[cmd_idx].pars, ccfile.cyc_sel, ccfile.array_idx, PARS_RW);
        }
        else if(remaining_line[0] == '-' && remaining_line[1] == '\n')
        {
            ccParsPrintAll(stdout, cmds[cmd_idx].name, cmds[cmd_idx].pars, ccfile.cyc_sel, ccfile.array_idx, PARS_RO);
        }
        else if(ccParseParName(cmd_idx, &remaining_line, &par_matched) == EXIT_FAILURE)
        {
            return(EXIT_FAILURE);
        }
        else if(remaining_line == NULL)
        {
            ccParsPrint(stdout, cmds[cmd_idx].name, par_matched, ccfile.cyc_sel, ccfile.array_idx);
        }
        else
        {
            uint32_t exit_status = ccParsGet(cmds[cmd_idx].name, par_matched, remaining_line);

            // If parameter is used by libreg then inform libreg that a parameter has been changed

            if(cmd_idx > 0 && (par_matched->flags & PARS_REG) != 0)
            {
                regMgrPars(&reg_mgr);
            }

            // If reading from stdin and a configuration parameter is changed then store it in a file

            if(ccfile.using_stdin && (par_matched->flags & PARS_CFG) != 0)
            {
                if(ccFileSaveConfigPar(cmds[cmd_idx].name, par_matched) == EXIT_FAILURE)
                {
                    return(EXIT_FAILURE);
                }
            }

            return(exit_status);
        }
    }
    return(EXIT_SUCCESS);
}



uint32_t ccCmdsHelp(uint32_t cmd_idx, char *remaining_line)
{
    if(ccParseNoMoreArgs(&remaining_line) == EXIT_FAILURE)
    {
        return(EXIT_FAILURE);
    }

    for(cmd_idx = 0 ; cmd_idx < N_CMDS ; cmd_idx++)
    {
        printf("%-*s %s\n", CC_MAX_CMD_NAME_LEN, cmds[cmd_idx].name, cmds[cmd_idx].help_message);
    }

    return(EXIT_SUCCESS);
}



uint32_t ccCmdsRead(uint32_t cmd_idx, char *remaining_line)
{
    uint32_t       exit_status = EXIT_SUCCESS;
    char           line[CC_MAX_FILE_LINE_LEN];
    FILE          *f;
    char          *arg;
    static char *  default_file_name = "ccrt";

    // Check that input file nesting limit has not been exceeded

    if(ccfile.input_idx >= (CC_INPUT_FILE_NEST_LIMIT - 1))
    {
        ccParsPrintError("input file nesting limit (%u) reached", CC_INPUT_FILE_NEST_LIMIT);
        return(EXIT_FAILURE);
    }

    // Get the first argument (if provided) and check that there are no more arguments after that

    arg = ccParseNextArg(&remaining_line);

    if(ccParseNoMoreArgs(&remaining_line))
    {
        return(EXIT_FAILURE);
    }

    // If no argument supplied then default to reading stdin

    if(arg == NULL)
    {
        // If already reading from stdin or from a file then report an error

        if(ccfile.input_idx > 0)
        {
            ccParsPrintError("already reading from a file or from stdin");
            return(EXIT_FAILURE);
        }

        // Read from stdin

        f = stdin;
        ccfile.using_stdin = true;
        ccfile.input_idx++;
        ccfile.input[ccfile.input_idx].line_number = 0;
        ccfile.input[ccfile.input_idx].path        = default_file_name;

        // Display status and prompt on new lines

        ccStatus(false);
    }
    else
    {
        // Try to open named file

        f = fopen(arg, "r");

        if(f == NULL)
        {
             ccParsPrintError("opening file '%s' : %s (%d)", ccParseAbbreviateArg(arg), strerror(errno), errno);
             return(EXIT_FAILURE);
        }

        if(cmd_idx != 0)
        {
            printf("Reading parameters from '%s'\n", arg);
        }

        // Stack new file information

        ccfile.input_idx++;
        ccfile.input[ccfile.input_idx].line_number = 1;
        ccfile.input[ccfile.input_idx].path        = arg;
    }

    // Process all lines from the new file or from stdin

    while(fgets(line, CC_MAX_FILE_LINE_LEN, f) != NULL)
    {
        // Check if line was too long (2 chars are needed for newline and terminating nul)

        if(strlen(line) >= (CC_MAX_FILE_LINE_LEN-1) && line[CC_MAX_FILE_LINE_LEN-2] != '\n')
        {
            int   input_ch;

            ccParsPrintError("line exceeds maximum length (%u)", CC_MAX_FILE_LINE_LEN-2);
            exit_status = EXIT_FAILURE;

            // Purge the rest of the line

            while((input_ch = fgetc(f)) != '\n' && input_ch != EOF);
        }
        else
        {
            // Parse the input line

            exit_status = ccParseLine(line);
        }

        // Print prompt when using stdin

        if(ccfile.using_stdin)
        {
            ccStatus(ccfile.empty_line);
        }
        else // else when reading from file, break out if error reported
        {
            if(exit_status == EXIT_FAILURE)
            {
                break;
            }

            ccfile.input[ccfile.input_idx].line_number++;
        }
    }

    // If reading from a file then close it

    if(ccfile.using_stdin)
    {
        ccfile.using_stdin = false;
    }
    else
    {
        fclose(f);
        ccfile.input_idx--;
    }

    return(exit_status);
}



uint32_t ccCmdsArm(uint32_t cmd_idx, char *remaining_line)
{
    char    *arg;
    int32_t cyc_sel;

    // If no arguments provided, try to arm every cycle selector

    if(remaining_line == NULL)
    {
        for(cyc_sel = 0 ; cyc_sel <= CC_MAX_CYC_SEL ; cyc_sel++)
        {
            ccInitFunction(cyc_sel);
        }
    }
    else    // Treat arguments as a series of cycle selectors
    {
        while((arg = ccParseNextArg(&remaining_line)) != NULL)
        {
            char    *remaining_arg;

            cyc_sel = strtol(arg, &remaining_arg, 10);

            if(*remaining_arg != '\0' || errno != 0 || cyc_sel < 0 || cyc_sel > CC_MAX_CYC_SEL)
            {
                ccParsPrintError("invalid cycle selector: %s", ccParseAbbreviateArg(arg));
                return(EXIT_FAILURE);
            }

            // Try to arm function for this cycle selector

            if(ccInitFunction(cyc_sel) == EXIT_FAILURE)
            {
                return(EXIT_FAILURE);
            }
        }
    }
    return(EXIT_SUCCESS);
}



uint32_t ccCmdsWait(uint32_t cmd_idx, char *remaining_line)
{
    char      *arg;
    useconds_t sleep_time_us;

    // If no arguments provided, try to arm every cycle selector

    if(remaining_line == NULL)
    {
        // Default is 1s

        sleep_time_us = 1000000;
    }
    else    // Treat arguments as a series of cycle selectors
    {
        char    *remaining_arg;
        double   value;

        arg = ccParseNextArg(&remaining_line);

        if(ccParseNoMoreArgs(&remaining_line))
        {
            return(EXIT_FAILURE);
        }

        value = strtod(arg, &remaining_arg);

        if(*remaining_arg != '\0' || errno != 0 || value <= 0.0 || value > 1000.0)
        {
            ccParsPrintError("invalid sleep time: %s", ccParseAbbreviateArg(arg));
            return(EXIT_FAILURE);
        }

        sleep_time_us = (useconds_t)(value * 1000000);
    }

    // Adjust sleep time when running from script faster than real-time

    if(ccfile.using_stdin == false)
    {
        sleep_time_us /= CC_OFFLINE_ACCELERATION;
    }

    // Sleep for specified time

    usleep(sleep_time_us);

    return(EXIT_SUCCESS);
}



uint32_t ccCmdsLog(uint32_t cmd_idx, char *remaining_line)
{
    if(ccParseNoMoreArgs(&remaining_line) == EXIT_FAILURE)
    {
        return(EXIT_FAILURE);
    }

    // Write log data to file

    if(ccFileWriteLog() == EXIT_FAILURE)
    {
        return(EXIT_FAILURE);
    }

    // Check if any bad values have been seend

    ccLogReportBadValues(&breg_log);
    ccLogReportBadValues(&ireg_log);
    ccLogReportBadValues(&meas_log);

    return(EXIT_SUCCESS);
}



uint32_t ccCmdsExit(uint32_t cmd_idx, char *remaining_line)
{
    if(ccParseNoMoreArgs(&remaining_line) == EXIT_FAILURE)
    {
        return(EXIT_FAILURE);
    }

    // If processing commands from the command line or stdin then quit immediately

    if(ccfile.input_idx == 0 || ccfile.input[ccfile.input_idx].line_number == 0)
    {
        ccCmdsQuit(0, remaining_line);
    }

    // Return failure to close current file

    return(EXIT_FAILURE);
}



uint32_t ccCmdsQuit(uint32_t cmd_idx, char *remaining_line)
{
    if(ccParseNoMoreArgs(&remaining_line) == EXIT_FAILURE)
    {
        return(EXIT_FAILURE);
    }

    puts("Stopping ccrt.");
    exit(EXIT_SUCCESS);
}

// EOF
