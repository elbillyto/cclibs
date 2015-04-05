/*!
 * @file  ccrt/scr/ccPars.c
 *
 * @brief ccrt parameter processing functions
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
#include <errno.h>

// Include cctest program header files

#include "ccCmds.h"
#include "ccRt.h"
#include "ccParse.h"
#include "ccFile.h"
#include "ccRef.h"
#include "ccRun.h"



uint32_t ccParsGet(char *cmd_name, struct CCpars *par, char *remaining_line)
/*
 * This function will try to interpret arguments remaining on the input line as values belonging to
 * a command parameter. It supports different interpretations of multiple arguments, e.g.:
 *
 * REF FUNCTION() SINE                   Sets REF FUNCTION(1-16)
 * REF FUNCTION(5) PLEP TABLE PPPL       Sets REF FUNCTION(5) (6) and (7)
 * TABLE REF(5) 1 2 3 4                  Sets TABLE REF(5)
 *
 * If a cycle selector is supplied and the parameter is scalar then multiple values will apply to
 * consecutive cycle selectors.
 */
{
    int32_t              int_value;
    uint32_t             array_idx;
    uint32_t             cyc_sel;
    uint32_t             cyc_sel_inc  = 0;
    uint32_t             cyc_sel_from = 0;
    uint32_t             cyc_sel_to   = 0;
    uint32_t             num_pars     = 0;
    uint32_t             max_pars;
    double               double_value;
    char                *remaining_arg;
    char                *arg;
    size_t               arg_len;
    struct CCpars_enum  *par_enum;
    struct CCpars_enum  *par_enum_matched;
    union CCvalue_p        value_p;

    // Prepare array index

    array_idx = (ccfile.array_idx == CC_NO_INDEX) ? 0 : ccfile.array_idx;

    // Prepare cycle selector range

    if(ccfile.cyc_sel == CC_NO_INDEX || par->cyc_sel_step == 0)
    {
        max_pars = par->max_num_elements - array_idx;
    }
    else if(ccfile.cyc_sel == CC_ALL_CYCLES)
    {
        cyc_sel_from  = 0;
        cyc_sel_to    = CC_MAX_CYC_SEL;
        max_pars      = par->max_num_elements - array_idx;
    }
    else // cyc_sel provided
    {
        cyc_sel_from = cyc_sel_to = ccfile.cyc_sel;

        if(par->max_num_elements == 1)
        {
            cyc_sel_inc = 1;
            max_pars    = CC_NUM_CYC_SELS - cyc_sel_from;
        }
        else
        {
            max_pars = par->max_num_elements - array_idx;
        }
    }

    // Reset errno because strtod does not set it to zero on success

    errno = 0;

    // Try to parse the arguments to set the parameter values

    while((arg = ccParseNextArg(&remaining_line)) != NULL)
    {
        if(num_pars >= max_pars)
        {
            ccParsPrintError("too many values for %s %s (%u max)", cmd_name, par->name, max_pars);
            return(EXIT_FAILURE);
        }

        arg_len = strlen(arg);

        // If only argument is '?' then print parameter value and options for enums

        if(num_pars == 0 && arg_len == 1 && *arg == '?' && remaining_line == NULL)
        {
            ccParsPrint(stdout, cmd_name, par, ccfile.cyc_sel, ccfile.array_idx);

            if(par->type == PAR_ENUM)
            {
                for(par_enum = par->ccpars_enum, par_enum_matched = NULL ; par_enum->string != NULL ; par_enum++)
                {
                    printf("%s ",par_enum->string);
                }

                putchar('\n');
            }

            return(EXIT_SUCCESS);
        }

        // Converter the parameter from ASCII according to the type of parameter

        switch(par->type)
        {
        case PAR_DOUBLE:    // Only used for read-only parameters
        case PAR_BOOL:

            break;

        case PAR_UNSIGNED:

            int_value = strtol(arg, &remaining_arg, 10);

            if(*remaining_arg != '\0' || errno != 0 || int_value < 0)
            {
                ccParsPrintError("invalid integer for %s %s[%u]: '%s'",
                        cmd_name, par->name, array_idx, arg);
                return(EXIT_FAILURE);
            }

            for(cyc_sel = cyc_sel_from ; cyc_sel <= cyc_sel_to ; cyc_sel++)
            {
                value_p.c = par->value_p.c + cyc_sel * par->cyc_sel_step;
                value_p.u[array_idx] = (uint32_t)int_value;
            }
            break;

        case PAR_FLOAT:

            double_value = strtod(arg, &remaining_arg);

            // Protect against double value that cannot fit in a float

            if(*remaining_arg != '\0' || errno != 0 || double_value >= FLT_MAX || double_value <= -FLT_MAX)
            {
                ccParsPrintError("invalid float for %s %s[%u]: '%s'",
                        cmd_name, par->name, array_idx, arg);
                return(EXIT_FAILURE);
            }

            for(cyc_sel = cyc_sel_from ; cyc_sel <= cyc_sel_to ; cyc_sel++)
            {
                value_p.c = par->value_p.c + cyc_sel * par->cyc_sel_step;
                value_p.f[array_idx] = (float)double_value;
            }
            break;

        case PAR_STRING:

            // Limit string lengths because they are used in path names

            if(arg_len > 32)
            {
                ccParsPrintError("invalid string length (%u) for %s %s[%u]: '%s' (32 max)",
                            arg_len, cmd_name, par->name, array_idx, ccParseAbbreviateArg(arg));
                return(EXIT_FAILURE);
            }

            // Free and reallocate space for string argument
            // On first call, the pointer is NULL which is ignored by free()

            for(cyc_sel = cyc_sel_from ; cyc_sel <= cyc_sel_to ; cyc_sel++)
            {
                value_p.c = par->value_p.c + cyc_sel * par->cyc_sel_step;
                free(value_p.s[array_idx]);
                value_p.s[array_idx] = strcpy(malloc(arg_len+1),arg);
            }
            break;

        case PAR_ENUM:

            // Compare first argument against list of enum strings for the parameter

            for(par_enum = par->ccpars_enum, par_enum_matched = NULL ; par_enum->string != NULL ; par_enum++)
            {
                // If argument matches start or all of an enum string

                if(strncasecmp(par_enum->string, arg, arg_len) == 0)
                {
                    // If first match, remember enum

                    if(par_enum_matched == NULL)
                    {
                        par_enum_matched = par_enum;

                        // If match is exact then break out of search

                        if(strcasecmp(par_enum->string, arg) == 0)
                        {
                            break;
                        }
                    }
                    else // else second match so report error
                    {
                        ccParsPrintError("ambiguous enum for %s %s[%u]: '%s'",
                                         cmd_name, par->name, array_idx, arg);
                        return(EXIT_FAILURE);
                    }
                }
            }

            if(par_enum_matched == NULL)
            {
                ccParsPrintError("unknown enum for %s %s[%u]: '%s'",
                                 cmd_name, par->name, array_idx, ccParseAbbreviateArg(arg));
                return(EXIT_FAILURE);
            }

            if(par_enum_matched->flags & CC_ENUM_READ_ONLY)
            {
                ccParsPrintError("read-only enum for %s %s[%u]: '%s'", cmd_name, par->name, array_idx, arg);
                return(EXIT_FAILURE);
            }

            for(cyc_sel = cyc_sel_from ; cyc_sel <= cyc_sel_to ; cyc_sel++)
            {
                value_p.c = par->value_p.c + cyc_sel * par->cyc_sel_step;
                value_p.u[array_idx] = par_enum_matched->value;
            }
            break;
        }

        if(cyc_sel_inc == 0)
        {
            array_idx++;

            // Update num_elements if the array length is growing with each new argument

            for(cyc_sel = cyc_sel_from ; cyc_sel <= cyc_sel_to ; cyc_sel++)
            {
                if(array_idx > par->num_elements[cyc_sel])
                {
                    par->num_elements[cyc_sel] = array_idx;
                }
            }
        }
        else
        {
            cyc_sel_from++;
            cyc_sel_to++;
        }

        num_pars++;
    }

    // If parameter length is NOT fixed

    if((par->flags & PARS_FIXLEN) == 0 && par->max_num_elements > 1)
    {
        for(cyc_sel = cyc_sel_from ; cyc_sel <= cyc_sel_to ; cyc_sel++)
        {
            uint32_t num_elements = par->num_elements[cyc_sel];

            // If array has shrunk then zero the trailing values and update num_elements

            if(array_idx < num_elements &&
              (ccfile.array_idx == CC_NO_INDEX || array_idx > (ccfile.array_idx + 1)))
            {
                value_p.c = par->value_p.c + cyc_sel * par->cyc_sel_step;

                memset(&value_p.c[ccpars_sizeof_type[par->type] * array_idx],
                       0,
                       ccpars_sizeof_type[par->type] * (num_elements - array_idx));

                par->num_elements[cyc_sel] = array_idx;
            }
        }
    }

    return(EXIT_SUCCESS);
}



int32_t ccParsEnumIndex(struct CCpars_enum *par_enum, uint32_t value)
{
    int32_t idx;

    // Search enum for a matching value

    for(idx = 0 ; par_enum[idx].string != NULL && par_enum[idx].value != value; idx++);

    // Return -1 if no match found

    return(par_enum[idx].string != NULL ? idx : -1);
}



char * ccParsEnumString(struct CCpars_enum *par_enum, uint32_t value)
{
    // Search enum for a matching value

    while(par_enum->string != NULL && par_enum->value != value)
    {
        par_enum++;
    }

    // Return "invalid" if no match found

    return(par_enum->string != NULL ? par_enum->string : "invalid");
}



static void ccParsPrintElement(FILE *f, struct CCpars *par, uint32_t cyc_sel, uint32_t array_idx)
{
    if(array_idx < par->num_elements[cyc_sel])
    {
        union CCvalue_p value_p;

        value_p.c = par->value_p.c + cyc_sel * par->cyc_sel_step;

        switch(par->type)
        {
        case PAR_UNSIGNED:

            fprintf(f, PARS_INT_FORMAT, value_p.u[array_idx]);
            break;

        case PAR_FLOAT:

            fprintf(f, PARS_FLOAT_FORMAT, value_p.f[array_idx]);
            break;

        case PAR_DOUBLE:

            fprintf(f, PARS_TIME_FORMAT, value_p.d[array_idx]);
            break;

        case PAR_STRING:

            fprintf(f, PARS_STRING_FORMAT, value_p.s[array_idx]);
            break;

        case PAR_ENUM:

            fprintf(f, PARS_STRING_FORMAT, ccParsEnumString(par->ccpars_enum, value_p.u[array_idx]));
            break;

        case PAR_BOOL:

            fprintf(f, PARS_STRING_FORMAT, value_p.b[array_idx] ? "TRUE" : "FALSE");
            break;
        }
    }
}



void ccParsPrint(FILE *f, char *cmd_name, struct CCpars *par, uint32_t cyc_sel, uint32_t array_idx)
{
    uint32_t  num_elements;
    uint32_t  num_characters;
    uint32_t  idx_from;
    uint32_t  idx_to;
    uint32_t  idx_local;
    uint32_t  cyc_sel_from;
    uint32_t  cyc_sel_to;

    // Determine cycle selector range from cyc_sel

    if(par->cyc_sel_step == 0)
    {
        if(cyc_sel == CC_NO_INDEX || cyc_sel == CC_ALL_CYCLES || cyc_sel == 0)
        {
            cyc_sel_from = cyc_sel_to = 0;
        }
        else // return immediately if non-cyc_sel parameters when non-zero cyc_sel supplied
        {
            return;
        }
    }
    else
    {
        if(cyc_sel == CC_ALL_CYCLES)
        {
            cyc_sel_from = 0;
            cyc_sel_to   = CC_MAX_CYC_SEL;
        }
        else if(cyc_sel == CC_NO_INDEX)
        {
            cyc_sel_from = cyc_sel_to = 0;
        }
        else
        {
            cyc_sel_from = cyc_sel_to = cyc_sel;
        }
    }

    // Print values for each cycle selector within the range2

    for(cyc_sel = cyc_sel_from ; cyc_sel <= cyc_sel_to ; cyc_sel++)
    {
        num_elements = par->num_elements[cyc_sel];

        if(num_elements > 0)
        {
            // Print parameter name

            num_characters = fprintf(f,"%-*s %s",CC_MAX_CMD_NAME_LEN,cmd_name,par->name);

            // Print cycle selector if non zero

            if(cyc_sel > 0)
            {
                num_characters += fprintf(f, "(%u)", cyc_sel);
            }

            // If array index provided, then just display this one element

            if(array_idx != CC_NO_INDEX)
            {
                idx_from = idx_to = array_idx;
                num_characters += fprintf(f, "[%u]", array_idx);
            }
            else // else display all elements
            {
                idx_from = 0;
                idx_to   = num_elements - 1;
            }

            // Pad with spaces to align values colunn

            while(++num_characters < CC_MAX_PAR_NAME_LEN)
            {
                fputc(' ',f);
            }

            // Print the values for the specified array range

            for(idx_local = idx_from ; idx_local <= idx_to ; idx_local++)
            {
                ccParsPrintElement(f, par, cyc_sel, idx_local);
            }

            fputc('\n',f);
        }
    }
}



void ccParsPrintAll(FILE *f, char *cmd_name, struct CCpars *par, uint32_t cyc_sel, uint32_t array_idx, uint32_t flags)
{
    // If ALL_CYCLES specifed by ()

    if(cyc_sel == CC_ALL_CYCLES)
    {
        struct CCpars *pars = par;

        for(cyc_sel = 0 ; cyc_sel <= CC_MAX_CYC_SEL ; cyc_sel++)
        {
            for(par = pars ; par->name != NULL ; par++)
            {
                if(par->flags & flags)
                {
                    ccParsPrint(f, cmd_name, par, cyc_sel, array_idx);
                }
            }
        }
    }
    else // Use supplied cyc_sel
    {
        while(par->name != NULL)
        {
            if(par->flags & flags)
            {
                ccParsPrint(f, cmd_name, par, cyc_sel, array_idx);
            }

            par++;
        }
    }
}



void ccParsPrintError(const char * format, ...)
{
    va_list     argv;

    // If processing command line arguments

    if(ccfile.input_idx == 0)
    {
        printf("Error at argument %u - ",ccfile.input[0].line_number);
    }
    else
    {
        // If reading from stdin

        if(ccfile.input[ccfile.input_idx].line_number == 0)
        {
            printf("Error - ");
        }
        else // else reading from file
        {
            printf("Error at %s:%u - ",
                    ccfile.input[ccfile.input_idx].path,
                    ccfile.input[ccfile.input_idx].line_number);
        }
    }

    // Print error message

    va_start(argv, format);
    vprintf(format, argv);
    va_end(argv);

    // Write newline

    putchar('\n');
}

// EOF
