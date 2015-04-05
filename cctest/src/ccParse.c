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
#include <stdlib.h>
#include <string.h>
#include <errno.h>

// Include cctest program header files

#include "ccCmds.h"
#include "ccTest.h"
#include "ccParse.h"
#include "ccFile.h"

/*---------------------------------------------------------------------------------------------------------*/
static uint32_t ccParseIndex(char **line, char delimiter, uint32_t *index)
/*---------------------------------------------------------------------------------------------------------*\
  This function will try to translate a cycle selector in () or an array index in [].
\*---------------------------------------------------------------------------------------------------------*/
{
    long int value;
    char    *index_string;
    char    *remaining_string;

    // Skip leading white space before index

    index_string = *line + strspn(*line, " \t");

    // Report failure if end of line reached

    if(*index_string == '\0')
    {
        return(EXIT_FAILURE);
    }

    // If delimiters contain no value

    if(*index_string == delimiter)
    {
        if(delimiter == ')')
        {
            // Empty cycle selector "()" means "all cycles"

            *index = CC_ALL_CYCLES;
            *line = index_string + 1;

            return(EXIT_SUCCESS);
        }

        // Empty array index "[]" is not allowed

        return(EXIT_FAILURE);
    }

    // Try to translate next characters into an integer

    errno = 0;

    value = strtol(index_string, &remaining_string, 10);

    // Skip any trailing white space

    index_string = remaining_string + strspn(remaining_string, " \t");

    // Check trailing delimiter is valid and that integer was successfully converted,
    // and if it is a cycle selector, that it is not out of range

     if(*index_string != delimiter                      ||
         errno != 0                                     ||
         value  < 0                                     ||
        ((delimiter == ')' && value >  CC_MAX_CYC_SEL)) ||
        ((delimiter == ']' && value >= TABLE_LEN     )))
     {
         return(EXIT_FAILURE);
     }

     *index = (uint32_t)value;
     *line  = index_string + 1;
     return(EXIT_SUCCESS);
}
/*---------------------------------------------------------------------------------------------------------*/
static uint32_t ccParseArg(char **line, size_t *arg_len)
/*---------------------------------------------------------------------------------------------------------*/
{
    char     delimiter;
    char    *remaining_string = *line;

    *arg_len = strcspn(remaining_string, CC_ARG_DELIMITER "([");

    remaining_string += *arg_len;

    delimiter = *remaining_string;

    if(delimiter != '\0')
    {
        // Nul terminate command string

        *(remaining_string++) = '\0';

        // If cycle selector is provided then parse the value

        if(delimiter == '(')
        {
            if(ccParseIndex(&remaining_string, ')', &ccfile.cyc_sel) == EXIT_FAILURE)
            {
                ccParsPrintError("invalid command cycle selector");
                return(EXIT_FAILURE);
            }

            delimiter = *(remaining_string++);
        }

        // If array index is provided then parse the value

        if(delimiter == '[')
        {
            if(ccParseIndex(&remaining_string, ']', &ccfile.array_idx) == EXIT_FAILURE)
            {
                ccParsPrintError("invalid command array index");
                return(EXIT_FAILURE);
            }
        }

        remaining_string += strspn(remaining_string, CC_ARG_DELIMITER);
    }

    // If no more arguments then set line to NULL

    if(*remaining_string == '\0')
    {
        remaining_string = NULL;
    }

    *line = remaining_string;

    return(EXIT_SUCCESS);
}
/*---------------------------------------------------------------------------------------------------------*/
uint32_t ccParseLine(char *line)
/*---------------------------------------------------------------------------------------------------------*\
  This function will try to set the current directory using the supplied parameter
\*---------------------------------------------------------------------------------------------------------*/
{
    int32_t        idx;
    int32_t        cmd_idx;
    char          *command;
    char          *remaining_string;
    size_t         command_len;
    struct cccmds *cmd;

    // Skip leading white space and ignore empty lines or comment (#) lines

    remaining_string = command = line + strspn(line, " \t");

    if(*command == '\n' || *command == '\0' || *command == '#')
    {
        return(EXIT_SUCCESS);
    }

    // Initialise cycle selectors and array indexes

    ccfile.cyc_sel   = CC_NO_INDEX;
    ccfile.array_idx = CC_NO_INDEX;

    // Get first argument from line (command name) and optional cycle selector and array index

    if(ccParseArg(&remaining_string, &command_len) == EXIT_FAILURE)
    {
        return(EXIT_FAILURE);
    }

    // Compare first argument against list of commands

    for(cmd = cmds, cmd_idx = -1, idx = 0 ; cmd->name != NULL ; cmd++, idx++)
    {
         // If command argument matches start or all of a command

         if(strncasecmp(cmd->name, command, command_len) == 0)
         {
             // If first match, remember command index

             if(cmd_idx == -1)
             {
                 cmd_idx = idx;
             }
             else // else second match so report error
             {
                 ccParsPrintError("ambiguous command '%s'", command);
                 return(EXIT_FAILURE);
             }
         }
    }

    // If unambiguous match with a command, run the associated command function

    if(cmd_idx >= 0)
    {
        // If command is not a parameter group

        if(cmds[cmd_idx].cmd_func != ccCmdsPar)
        {
            // No cycle selector or array index may be supplied

            if(ccfile.cyc_sel != CC_NO_INDEX)
            {
                ccParsPrintError("unexpected command cycle selector");
                return(EXIT_FAILURE);
            }

            if(ccfile.array_idx != CC_NO_INDEX)
            {
                ccParsPrintError("unexpected command array index");
                return(EXIT_FAILURE);
            }
        }

        return(cmds[cmd_idx].cmd_func(cmd_idx, remaining_string));
    }
    else
    {
         ccParsPrintError("unknown command '%s'", ccParseAbbreviateArg(command));
         return(EXIT_FAILURE);
    }

    return(EXIT_SUCCESS);
}
/*---------------------------------------------------------------------------------------------------------*/
uint32_t ccParseParName(uint32_t cmd_idx, char **remaining_line, struct CCpars **par_matched)
/*---------------------------------------------------------------------------------------------------------*\
  This function analyse the next argument on the line which must be empty or a valid parameter name
\*---------------------------------------------------------------------------------------------------------*/
{
    size_t              par_string_len;
    char               *par_string = *remaining_line;
    struct CCpars      *par;

    // When a parameter name is provided, the command may not have a cycle selector or array index

    if(ccfile.cyc_sel != CC_NO_INDEX)
    {
        ccParsPrintError("unexpected command cycle selector");
        return(EXIT_FAILURE);
    }

    if(ccfile.array_idx != CC_NO_INDEX)
    {
        ccParsPrintError("unexpected command array index");
        return(EXIT_FAILURE);
    }

    // Get next argument which is the parameter name and allow cycle selector and array index to be specified

    if(ccParseArg(remaining_line, &par_string_len) == EXIT_FAILURE)
    {
        return(EXIT_FAILURE);
    }

    // Analyse argument to see if it is a known parameter name for the command.

    for(par = cmds[cmd_idx].pars, *par_matched = NULL ; par->name != NULL ; par++)
    {
         // If command argument matches start or all of a command

        if(strncasecmp(par->name, par_string, par_string_len) == 0)
        {
            // If argument exactly matches a parameter name then take use it

            if(strlen(par->name) == par_string_len)
            {
                *par_matched = par;
                break;
            }

            // If first partial match, remember command index

            if(*par_matched == NULL)
            {
                *par_matched = par;
            }
            else // else second partial match so report error
            {
                ccParsPrintError("ambiguous %s parameter '%s'", cmds[cmd_idx].name, par_string);
                return(EXIT_FAILURE);
            }
        }
    }

    if(*par_matched == NULL)
    {
        ccParsPrintError("unknown parameter for %s: '%s'", cmds[cmd_idx].name, ccParseAbbreviateArg(par_string));
        return(EXIT_FAILURE);
    }

    return(EXIT_SUCCESS);
}
/*---------------------------------------------------------------------------------------------------------*/
char * ccParseNextArg(char **remaining_line)
/*---------------------------------------------------------------------------------------------------------*\
  This function will return the next argument in the line, delimited by white space or a comma. It returns
  NULL if there are no more arguments and *remaining_line points the start of the next argument, or is NULL
  if there are no more arguments.
\*---------------------------------------------------------------------------------------------------------*/
{
    char *arg = *remaining_line;
    char *remaining_string;

    // if more arguments remain on the line

    if(arg != NULL)
    {
        remaining_string = arg + strcspn(arg, CC_ARG_DELIMITER);

        // Nul terminate the new argument and skip trailing delimiter characters

        if(*remaining_string != '\0')
        {
            *(remaining_string++) = '\0';

            remaining_string += strspn(remaining_string, CC_ARG_DELIMITER);
        }

        // If no more arguments on the line then set *remaining_line to NULL

        if(*remaining_string == '\0')
        {
            *remaining_line = NULL;
        }
        else
        {
            *remaining_line = remaining_string;
        }
    }

    return(arg);
}

/*---------------------------------------------------------------------------------------------------------*/
char *ccParseAbbreviateArg(char *arg)
/*---------------------------------------------------------------------------------------------------------*\
  This function will check if an argument is too long and will truncate with "..." if it is.  This makes 
  it safe to print.
\*---------------------------------------------------------------------------------------------------------*/
{
    if(strlen(arg) > CC_ABBREVIATED_ARG_LEN)
    {
        arg[CC_ABBREVIATED_ARG_LEN-4] = '.';
        arg[CC_ABBREVIATED_ARG_LEN-3] = '.';
        arg[CC_ABBREVIATED_ARG_LEN-2] = '.';
        arg[CC_ABBREVIATED_ARG_LEN-1] = '\0';
    }

    return(arg);
}
/*---------------------------------------------------------------------------------------------------------*/
uint32_t ccParseNoMoreArgs(char **remaining_line)
/*---------------------------------------------------------------------------------------------------------*\
  This function will check that there are no more arguments on the line. If there are, it will print an
  error message.
\*---------------------------------------------------------------------------------------------------------*/
{
    char *arg;

    arg = ccParseNextArg(remaining_line);

    if(arg != NULL)
    {
        ccParsPrintError("unexpected argument '%s'", ccParseAbbreviateArg(arg));
        return(EXIT_FAILURE);
    }

    return(EXIT_SUCCESS);
}

// EOF
