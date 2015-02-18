/*---------------------------------------------------------------------------------------------------------*\
  File:     ccCmds.c                                                                    Copyright CERN 2015

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

  Purpose:  Converter controls libraries test program parameter parsing functions

  Author:   Quentin.King@cern.ch
\*---------------------------------------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <errno.h>

// Include cctest program header files

#include "ccCmds.h"
#include "ccTest.h"
#include "ccParse.h"
#include "ccFile.h"
#include "ccLog.h"
#include "ccFlot.h"
#include "ccInit.h"
#include "ccRun.h"
#include "ccDebug.h"

/*---------------------------------------------------------------------------------------------------------*/
uint32_t ccCmdsHelp(uint32_t cmd_idx, char *remaining_line)
/*---------------------------------------------------------------------------------------------------------*\
  This function will print a help message listing all the commands
\*---------------------------------------------------------------------------------------------------------*/
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
/*---------------------------------------------------------------------------------------------------------*/
uint32_t ccCmdsCd(uint32_t cmd_idx, char *remaining_line)
/*---------------------------------------------------------------------------------------------------------*\
  This function will try to set the current directory using the supplied parameter
\*---------------------------------------------------------------------------------------------------------*/
{
    char   *arg;
    FILE   *f;
    char   *wd;
    char    cwd_buf[CC_PATH_LEN];

    arg = ccParseNextArg(&remaining_line);

    // If no argument supplied then use the base path for cctest project

    if(arg == NULL)
    {
        arg = ccfile.base_path;
    }
    else if(ccParseNoMoreArgs(&remaining_line) == EXIT_FAILURE)
    {
        return(EXIT_FAILURE);
    }

    // If changing directory fails, then report the error message

    if(chdir(arg) != EXIT_SUCCESS)
    {
        ccParsPrintError("changing directory to '%s' : %s (%d)",
                          ccParseAbbreviateArg(arg), strerror(errno), errno);
        return(EXIT_FAILURE);
    }

    // If reading from stdin, open CMD file to save the current working directory

    if(ccfile.cwd_file_path[0] != '\0')
    {
        f = fopen(ccfile.cwd_file_path,"w");

        if(f == NULL)
        {
            printf("Fatal - failed to open '%s' : %s (%d)\n", ccfile.cwd_file_path, strerror(errno), errno);
            exit(EXIT_FAILURE);
        }

        // Read the current working directory and write to the file

        wd = getcwd(cwd_buf, sizeof(cwd_buf));

        if(wd == NULL)
        {
            ccParsPrintError("getting current directory : %s (%d)", strerror(errno), errno);
            return(EXIT_FAILURE);
        }

        fputs(wd,f);

        fclose(f);
    }

    // Print new current working directory

    return(ccCmdsPwd(0, remaining_line));
}
/*---------------------------------------------------------------------------------------------------------*/
uint32_t ccCmdsPwd(uint32_t cmd_idx, char *remaining_line)
/*---------------------------------------------------------------------------------------------------------*\
  This function will display the current directory
\*---------------------------------------------------------------------------------------------------------*/
{
    char    cwd_buf[CC_PATH_LEN];
    char    *wd;

    // No arguments expected

    if(ccParseNoMoreArgs(&remaining_line) == EXIT_FAILURE)
    {
        return(EXIT_FAILURE);
    }

    // Get and print current working directory

    wd = getcwd(cwd_buf, sizeof(cwd_buf));

    if(wd == NULL)
    {
        ccParsPrintError("getting current directory : %s (%d)", strerror(errno), errno);
        return(EXIT_FAILURE);
    }

    puts(wd);

    return(EXIT_SUCCESS);
}
/*---------------------------------------------------------------------------------------------------------*/
uint32_t ccCmdsLs(uint32_t cmd_idx, char *remaining_line)
/*---------------------------------------------------------------------------------------------------------*\
  This function will display the contents of the current directory using the ls command.  It will supply
  any arguments provided to ls.
\*---------------------------------------------------------------------------------------------------------*/
{
    char ls_command[CC_MAX_FILE_LINE_LEN];

    // Construct ls command

    strcpy(ls_command, "ls ");

    if(remaining_line != NULL)
    {
        strcat(ls_command, remaining_line);
    }

    // Use system to run ls command

    if(system(ls_command) != 0)
    {
        return(EXIT_FAILURE);
    }

    return(EXIT_SUCCESS);
}
/*---------------------------------------------------------------------------------------------------------*/
uint32_t ccCmdsRead(uint32_t cmd_idx, char *remaining_line)
/*---------------------------------------------------------------------------------------------------------*\
  This function will try to read lines from stdin or from file named in the supplied parameter. It can be
  called recursively via the call to ccTestParseLine(). It protects against nesting open files too deeply.
\*---------------------------------------------------------------------------------------------------------*/
{
    uint32_t       exit_status = EXIT_SUCCESS;
    char           line[CC_MAX_FILE_LINE_LEN];
    char           input_ch;
    FILE          *f;
    char          *arg;
    static char *  default_file_name = "cctest";

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
        ccfile.input_idx++;
        ccfile.input[ccfile.input_idx].line_number = 0;
        ccfile.input[ccfile.input_idx].path        = default_file_name;

        // Try to recover and display saved working directory

        ccFileRecoverPath();
        ccCmdsPwd(0, remaining_line);
        printf(CC_PROMPT);
    }
    else
    {
        // If argument is "*" then read all files in current working directory

        if(strcmp(arg, "*") == 0)
        {
            return(ccFileReadAll());
        }

        // Try to open named file

        f = fopen(arg, "r");

        if(f == NULL)
        {
             ccParsPrintError("opening file '%s' : %s (%d)", ccParseAbbreviateArg(arg), strerror(errno), errno);
             return(EXIT_FAILURE);
        }

        printf("Reading parameters from '%s'\n", arg);

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

        if(f == stdin)
        {
            printf(CC_PROMPT);
        }
        else // else when reading from file, break out if error reported and stop on error is enabled
        {
            if(ccpars_global.stop_on_error == REG_ENABLED && exit_status == EXIT_FAILURE)
            {
                break;
            }

            ccfile.input[ccfile.input_idx].line_number++;
        }
    }

    // If reading from a file then close it

    if(f != stdin)
    {
        fclose(f);
        ccfile.input_idx--;
    }

    return(exit_status);
}
/*---------------------------------------------------------------------------------------------------------*/
uint32_t ccCmdsSave(uint32_t cmd_idx, char *remaining_line)
/*---------------------------------------------------------------------------------------------------------*\
  This function will save all the parameters to file
\*---------------------------------------------------------------------------------------------------------*/
{
    FILE *f;
    char *arg;
    char *default_filename = "cctest_pars";
    struct cccmds *cmd;

    arg = ccParseNextArg(&remaining_line);

    // If no argument supplied then use the default filename

    if(arg == NULL)
    {
        arg = default_filename;
    }
    else if(ccParseNoMoreArgs(&remaining_line) == EXIT_FAILURE)
    {
        return(EXIT_FAILURE);
    }

    // Try to open named file to write

    f = fopen(arg, "w");

    if(f == NULL)
    {
         ccParsPrintError("opening file '%s' : %s (%d)", ccParseAbbreviateArg(arg), strerror(errno), errno);
         return(EXIT_FAILURE);
    }

    printf("Writing all parameters to '%s'...",arg);

    fprintf(f,"# CCTEST v%.2f\n",CC_VERSION);

    for(cmd = cmds ; cmd->name != NULL ; cmd++)
    {
        if(cmd->pars != NULL)
        {
            fprintf(f, "\n# %s Parameters\n\n", cmd->name);

            ccParsPrintAll(f, cmd->name, cmd->pars, CC_ALL_CYCLES, CC_NO_INDEX);
        }
    }

    fputs("\n# EOF\n",f);
    fclose(f);
    puts("done.");
    return(EXIT_SUCCESS);
}
/*---------------------------------------------------------------------------------------------------------*/
uint32_t ccCmdsDebug(uint32_t cmd_idx, char *remaining_line)
/*---------------------------------------------------------------------------------------------------------*\
  This function will display the debug information for all the active parameters from the previous run.
\*---------------------------------------------------------------------------------------------------------*/
{
    // No arguments expected

    if(ccParseNoMoreArgs(&remaining_line))
    {
        return(EXIT_FAILURE);
    }

    // Print debug information from previous run to stdout

    ccDebugPrint(stdout);

    return(EXIT_SUCCESS);
}
/*---------------------------------------------------------------------------------------------------------*/
uint32_t ccCmdsRun(uint32_t cmd_idx, char *remaining_line)
/*---------------------------------------------------------------------------------------------------------*\
  This function will launch a run of the function generation and optionally simulate the voltage source
  and load.
\*---------------------------------------------------------------------------------------------------------*/
{
    char *filename;

    // No arguments expected

    if(ccParseNoMoreArgs(&remaining_line))
    {
        return(EXIT_FAILURE);
    }

    // Initialise run, the load model and the reference functions

    if(ccInitFunctions() == EXIT_FAILURE ||
      (ccpars_global.sim_load == REG_ENABLED && ccInitSimLoad() == EXIT_FAILURE))
    {
        // If STOP_ON_ERROR is DISABLED then dump debug data automatically to stdout

        if(ccpars_global.stop_on_error == REG_DISABLED)
        {
            ccDebugPrint(stdout);
        }

        return(EXIT_FAILURE);
    }

    // Enable signals that are to be logged

    ccLogInit();

    // Set filename

    filename = strcmp(ccpars_global.file, "*") != 0 ? ccpars_global.file : ccfile.input[ccfile.input_idx].path;

    // Open CSV output file if required

    if(ccpars_global.csv_output == REG_ENABLED)
    {
        if((ccfile.csv_file = ccFileOpenResultsFile("csv", filename, ccfile.csv_filename)) == NULL)
        {
            return(EXIT_FAILURE);
        }

        // Write CSV header line

        fputs("TIME",ccfile.csv_file);

        ccFileWriteCsvNames(&breg_log);
        ccFileWriteCsvNames(&ireg_log);
        ccFileWriteCsvNames(&meas_log);

        fputc('\n',ccfile.csv_file);
    }

    // Run the test

    if(ccpars_global.sim_load == REG_ENABLED)
    {
        // Generate functions and simulate voltage source and load and regulate if required

        printf("Running simulation to %s/%s/%s\n", ccpars_global.group, ccpars_global.project, filename);

        ccRunSimulation();
    }
    else
    {
        // Generate reference function only - no load simulation: this is just to test libfg functions

        if(ccpars_global.reverse_time == REG_DISABLED)
        {
            printf("Generating function(s) to %s/%s/%s\n",
                    ccpars_global.group, ccpars_global.project, filename);

            ccRunFuncGen();
        }
        else // Reverse time can be used with only one function
        {
            printf("Generating function with reverse time to %s/%s/%s\n",
                    ccpars_global.group, ccpars_global.project, filename);

            ccRunFuncGenReverseTime();
        }
    }

    // Close CSV output file

    if(ccpars_global.csv_output == REG_ENABLED)
    {
        if(fclose(ccfile.csv_file) != 0)
        {
            ccParsPrintError("closing file '%s' : %s (%d)", ccfile.csv_filename, strerror(errno), errno);
            return(EXIT_FAILURE);
        }
    }

    // Write HTML log file if required

    if(ccpars_global.html_output == REG_ENABLED)
    {
        FILE    *html_file;

        if((html_file = ccFileOpenResultsFile("html", filename, ccfile.html_filename)) == NULL)
        {
            return(EXIT_FAILURE);
        }

        ccFlot(html_file, filename);

        fclose(html_file);
    }

    // Write Debug file if required

    if(ccpars_global.debug_output == REG_ENABLED)
    {
        FILE            *ccd_file;
        struct cccmds   *cmd;

        if((ccd_file = ccFileOpenResultsFile("ccd", filename, ccfile.ccd_filename)) == NULL)
        {
            return(EXIT_FAILURE);
        }

        // Print debug data to debug file

        ccDebugPrint(ccd_file);

        // Print parameters to debug file

        for(cmd = cmds ; cmd->name != NULL ; cmd++)
        {
            fputc('\n', ccd_file);

            if(cmd->is_enabled == true)
            {
                ccParsPrintAll(ccd_file, cmd->name, cmd->pars, CC_ALL_CYCLES, CC_NO_INDEX);
            }
        }

        fclose(ccd_file);
    }

    // Report number of bad values logged - this can happen if the simulation parameters go crazy

    return(ccLogReportBadValues(&breg_log) |
           ccLogReportBadValues(&ireg_log) |
           ccLogReportBadValues(&meas_log));
}
/*---------------------------------------------------------------------------------------------------------*/
uint32_t ccCmdsPar(uint32_t cmd_idx, char *remaining_line)
/*---------------------------------------------------------------------------------------------------------*\
  This function will print or set parameters
\*---------------------------------------------------------------------------------------------------------*/
{
    // If no parameter name provided then print all parameter values

    if(remaining_line == NULL)
    {
        ccParsPrintAll(stdout, cmds[cmd_idx].name, cmds[cmd_idx].pars, ccfile.cyc_sel, ccfile.array_idx);
    }
    else // else parameter name argument provided
    {
        struct ccpars *par_matched;

        if(ccParseParName(cmd_idx, &remaining_line, &par_matched) == EXIT_FAILURE)
        {
            return(EXIT_FAILURE);
        }

        // If arguments provided then use them to set the parameter

        if(remaining_line == NULL)
        {
            ccParsPrint(stdout, cmds[cmd_idx].name, par_matched, ccfile.cyc_sel, ccfile.array_idx);
        }
        else
        {
            return(ccParsGet(cmds[cmd_idx].name, par_matched, remaining_line));
        }
    }
    return(EXIT_SUCCESS);
}
/*---------------------------------------------------------------------------------------------------------*/
uint32_t ccCmdsExit(uint32_t cmd_idx, char *remaining_line)
/*---------------------------------------------------------------------------------------------------------*\
  This function will stop reading from the open file or stop the program if reading from stdin
\*---------------------------------------------------------------------------------------------------------*/
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
/*---------------------------------------------------------------------------------------------------------*/
uint32_t ccCmdsQuit(uint32_t cmd_idx, char *remaining_line)
/*---------------------------------------------------------------------------------------------------------*\
  This function will stop execution of cctest immediately
\*---------------------------------------------------------------------------------------------------------*/
{
    if(ccParseNoMoreArgs(&remaining_line) == EXIT_FAILURE)
    {
        return(EXIT_FAILURE);
    }

    puts("Stopping cctest.");
    exit(EXIT_SUCCESS);
}
// EOF
