/*!
 * @file  ccrt/inc/ccFile.h
 *
 * @brief ccrt header file for ccFile.c
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
#define CC_TMP_LOG_FILE             "../logs/tmp"
#define CC_LOG_FILE                 "../logs/log.html"
#define CC_TMP_CONFIG_FILE          "../config/tmp"
#define CC_TMP_REF_FILE             "../ref/tmp"

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
    char                   *converter;
    bool                    empty_line;
    bool                    using_stdin;
};

CCFILE_EXT struct ccfile ccfile;

// Function declarations

uint32_t ccFileCwd                  (char *path);
uint32_t ccFileMakePath             (char *path);
uint32_t ccFileReadAll              (char *path);
uint32_t ccFileWriteLog             (void);
uint32_t ccFileSaveAllConfigPars    (void);
uint32_t ccFileSaveConfigPar        (char *cmd_name, struct CCpars *par);
uint32_t ccFileSaveRefPars          (uint32_t cyc_sel);

#endif
// EOF
