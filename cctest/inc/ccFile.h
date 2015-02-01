/*---------------------------------------------------------------------------------------------------------*\
  File:     cctest/inc/ccFile.h                                                         Copyright CERN 2015

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

  Purpose:  Header file for ccFile.c

  Authors:  Quentin.King@cern.ch
\*---------------------------------------------------------------------------------------------------------*/

#ifndef CCFILE_H
#define CCFILE_H

// GLOBALS should be defined in the source file where global variables should be defined

#ifdef GLOBALS
#define CCFILE_EXT
#else
#define CCFILE_EXT extern
#endif

// Constants

#define CC_PATH_LEN                 256
#define CC_INPUT_FILE_NEST_LIMIT    4
#define CC_CWD_FILE                 ".cctest_cwd"

// Global i/o structure

struct ccinput
{
    uint32_t                line_number;
    char                   *path;
};

struct ccfile
{
    uint32_t                input_idx;
    uint32_t                cyc_sel;
    uint32_t                array_idx;
    struct ccinput          input        [CC_INPUT_FILE_NEST_LIMIT];
    char                    base_path    [CC_PATH_LEN];
    char                    cwd_file_path[CC_PATH_LEN];
    FILE                   *csv_file;
};

CCFILE_EXT struct ccfile ccfile;

// Function declarations

uint32_t ccTestReadAllFiles     (void);
uint32_t ccTestMakePath         (char *path);
void     ccTestRecoverPath      (void);
void     ccTestGetBasePath      (char *argv0);

#endif
// EOF
