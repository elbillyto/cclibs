/*---------------------------------------------------------------------------------------------------------*\
  File:     cctest/inc/ccParse.h                                                        Copyright CERN 2015

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

  Purpose:  Header file for ccParse.c

  Authors:  Quentin.King@cern.ch
\*---------------------------------------------------------------------------------------------------------*/

#ifndef CCPARSE_H
#define CCPARSE_H

#include <stdint.h>

// Constants

#define CC_ARG_DELIMITER            ", \t\n"
#define CC_ABBREVIATED_ARG_LEN      20
#define CC_ALL_CYCLES               0xFFFFFFFE
#define CC_NO_INDEX                 0xFFFFFFFF

// Function declarations

uint32_t ccParseLine            (char *line);
uint32_t ccParseParName         (uint32_t cmd_idx, char **remaining_line, struct ccpars **par_matched);
char    *ccParseNextArg         (char **remaining_line);
char    *ccParseAbbreviateArg   (char *arg);
uint32_t ccParseNoMoreArgs      (char **remaining_line);

#endif
// EOF
