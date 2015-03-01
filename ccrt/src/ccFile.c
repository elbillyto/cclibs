/*!
 * @file  ccrt/scr/ccFile.c
 *
 * @brief ccrt file system related functions
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
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <ctype.h>
#include <string.h>
#include <libgen.h>
#include <errno.h>

// Include ccrt program header files

#include "ccCmds.h"
#include "ccFile.h"
#include "ccParse.h"
#include "ccFlot.h"
#include "ccRun.h"
#include "ccRef.h"



uint32_t ccFileCwd(char *path)
{
    if(chdir(path) != EXIT_SUCCESS)
    {
        printf("Error - changing directory to '%s' : %s (%d)", path, strerror(errno), errno);
        return(EXIT_FAILURE);
    }

    return(EXIT_SUCCESS);
}



uint32_t ccFileMakePath(char *path)
{
    struct stat      path_stat;

    // Get status of path

    if(stat(path, &path_stat) == -1)
    {
        // Path doesn't exist so try to create it

        printf("Creating '%s'\n", path);

        if(mkdir(path, S_IXUSR|S_IRUSR|S_IXGRP|S_IWUSR|S_IRGRP|S_IXOTH|S_IROTH))
        {
            printf("Error - failed to create new directory '%s': %s (%d)", path, strerror(errno), errno);
            return(EXIT_FAILURE);
        }

        return(EXIT_SUCCESS);
    }

    // Report error if the path is not a directory

    if(!S_ISDIR(path_stat.st_mode))
    {
        ccParsPrintError("path '%s' is not a valid directory", path);
        return(EXIT_FAILURE);
    }

    return(EXIT_SUCCESS);
}



uint32_t ccFileReadAll(char *path)
{
    struct dirent   *files;
    struct stat      dir_file_stat;
    DIR             *dp;
    char            *filename;

    printf("Reading %s parameter files\n",path);

    if(ccFileCwd(path) == EXIT_FAILURE)
    {
        return(EXIT_FAILURE);
    }

    // Try to open current working directory

    dp = opendir(".");

    if(dp == NULL)
    {
        printf("Error - ccFileReadAll failed to open current working directory : %s (%d)", strerror(errno), errno);
        exit(EXIT_FAILURE);
    }

    // Read contents of directory - this is not re-entrant because readdir_r() is not available in MinGW

    while((files = readdir(dp)) != NULL)
    {
        filename = files->d_name;

        // Get status of each file

        if(stat(filename, &dir_file_stat) == -1)
        {
            printf("Error - failed to stat '%s' in current working directory : %s (%d)", filename, strerror(errno), errno);
            return(EXIT_FAILURE);
        }

        // If regulation file

        if(S_ISREG(dir_file_stat.st_mode))
        {
            if(ccCmdsRead(0, filename) == EXIT_FAILURE)
            {
                // Close director and report failure

                closedir(dp);
                return(EXIT_FAILURE);
            }
        }
    }

    // Close directory and report success

    closedir(dp);

    // Return to previous path

    if(ccFileCwd("..") == EXIT_FAILURE)
    {
        return(EXIT_FAILURE);
    }

    return(EXIT_SUCCESS);
}



uint32_t ccFileWriteLog(void)
{
    FILE    *log_file;

    // Open temporary log file for writing

    log_file = fopen(CC_TMP_LOG_FILE, "w");

    if(log_file == NULL)
    {
         ccParsPrintError("opening file '%s' : %s (%d)", CC_TMP_LOG_FILE, strerror(errno), errno);
         return(EXIT_FAILURE);
    }

    // Create Flot chart for period up to the current moment

    ccFlot(log_file, ccfile.converter, ccrun.iter_time_s);

    fclose(log_file);

    // Rename file to replace old log

    if(rename(CC_TMP_LOG_FILE, CC_LOG_FILE) != 0)
    {
        ccParsPrintError("failed to rename '%s' to '%s' : %s (%d)", CC_TMP_LOG_FILE, CC_LOG_FILE, strerror(errno), errno);
        return(EXIT_FAILURE);
    }

    return(EXIT_SUCCESS);
}



uint32_t ccFileSaveAllConfigPars(void)
{
    struct cccmds *cmd;

    puts("Writing config parameter files");

    // Save all config parameters to separate config files

    for(cmd = cmds ; cmd->name != NULL ; cmd++)
    {
        struct ccpars *par = cmd->pars;

        if(par != NULL)
        {
            while(par->name != NULL)
            {
                if((par->flags & PARS_CFG) != 0)
                {
                    if(ccFileSaveConfigPar(cmd->name, par) == EXIT_FAILURE)
                    {
                         return(EXIT_FAILURE);
                    }
                }

                par++;
            }
        }
    }

    return(EXIT_SUCCESS);
}



uint32_t ccFileSaveConfigPar(char *cmd_name, struct ccpars *par)
{
    FILE    *par_file;
    char    path[CC_PATH_LEN];

    // Create file name using parameter name

    snprintf(path, CC_PATH_LEN, "../config/%s_%s", cmd_name, par->name);

    // Open temporary log file for writing

    par_file = fopen(CC_TMP_CONFIG_FILE, "w");

    if(par_file == NULL)
    {
         ccParsPrintError("opening file '%s' : %s (%d)", CC_TMP_CONFIG_FILE, strerror(errno), errno);
         return(EXIT_FAILURE);
    }

    // Write parameter name and value(s) to file

    ccParsPrint(par_file, cmd_name, par, CC_ALL_CYCLES, CC_NO_INDEX);

    fclose(par_file);

    // Rename file to replace file

    if(rename(CC_TMP_CONFIG_FILE, path) != 0)
    {
        ccParsPrintError("failed to rename '%s' to '%s' : %s (%d)", CC_TMP_CONFIG_FILE, path, strerror(errno), errno);
        return(EXIT_FAILURE);
    }

    return(EXIT_SUCCESS);
}



uint32_t ccFileSaveRefPars(uint32_t cyc_sel)
{
    FILE           *ref_file;
    uint32_t        cmd_idx;
    char            path[CC_PATH_LEN];

    // Create file name using parameter name

    snprintf(path, CC_PATH_LEN, "../ref/%02u", cyc_sel);

    // Open temporary log file for writing

    ref_file = fopen(CC_TMP_REF_FILE, "w");

    if(ref_file == NULL)
    {
         ccParsPrintError("opening file '%s' : %s (%d)", CC_TMP_REF_FILE,strerror(errno), errno);
         return(EXIT_FAILURE);
    }

    // Write REF parameters

    ccParsPrintAll(ref_file, cmds[CMD_REF].name, cmds[CMD_REF].pars, cyc_sel, CC_NO_INDEX, PARS_REF);

    // Write ref function parameters

    cmd_idx = funcs[ccpars_ref[cyc_sel].function].cmd_idx;

    ccParsPrintAll(ref_file, cmds[cmd_idx].name, cmds[cmd_idx].pars, cyc_sel, CC_NO_INDEX, PARS_RW);

    fclose(ref_file);

    // Rename file to replace file

    if(rename(CC_TMP_REF_FILE, path) != 0)
    {
        ccParsPrintError("failed to rename '%s' to '%s' : %s (%d)", CC_TMP_REF_FILE, path, strerror(errno), errno);
        return(EXIT_FAILURE);
    }

    return(EXIT_SUCCESS);
}

// EOF
