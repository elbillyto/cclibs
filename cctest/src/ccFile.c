/*---------------------------------------------------------------------------------------------------------*\
  File:     ccFile.c                                                                    Copyright CERN 2015

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
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <ctype.h>
#include <string.h>
#include <libgen.h>
#include <errno.h>

// Declare all variables in ccTest.c

// Include cctest program header files

#include "ccCmds.h"
#include "ccFile.h"

// mkdir command

// On Windows the system() command in MinGW uses cmd.exe which finds the Windows mkdir in the path
// instead of the msys/bin/mkdir. To work around this, a second copy of mkdir is created in msys/bin
// which is used on Windows systems.

#ifdef __MINGW32__
#define MKDIR   "mkdir2"
#else
#define MKDIR   "mkdir"
#endif

/*---------------------------------------------------------------------------------------------------------*/
uint32_t ccFileReadAll(void)
/*---------------------------------------------------------------------------------------------------------*\
  This function read the current working directory and the try to read each file in it.
\*---------------------------------------------------------------------------------------------------------*/
{
    struct dirent   *files;
    struct stat      dir_file_stat;
    DIR             *dp;
    char            *filename;

    // Try to open current working directory

    dp = opendir(".");

    if(dp == NULL)
    {
        printf("Fatal - failed to open current working directory : %s (%d)", strerror(errno), errno);
        exit(EXIT_FAILURE);
    }

    // Read contents of directory - this is not re-entrant because readdir_r() is not available in MinGW

    while((files = readdir(dp)) != NULL)
    {
        filename = files->d_name;

        // Get status of each file

        if(stat(filename, &dir_file_stat) == -1)
        {
            printf("Fatal - failed to stat '%s' in current working directory : %s (%d)", filename, strerror(errno), errno);
            exit(EXIT_FAILURE);
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

    if(errno != 0)
    {
        printf("Fatal - failed to read current working directory : %s (%d)", strerror(errno), errno);
        exit(EXIT_FAILURE);
    }

    // Close directory and report success

    closedir(dp);
    return(EXIT_SUCCESS);
}
/*---------------------------------------------------------------------------------------------------------*/
uint32_t ccFileMakePath(char *path)
/*---------------------------------------------------------------------------------------------------------*\
  This function will use mkdir -p to create a path if it doesn't exist
\*---------------------------------------------------------------------------------------------------------*/
{
    struct stat      path_stat;
    char             mkdir_cmd[CC_PATH_LEN];

    // Get status of path

    if(stat(path, &path_stat) == -1)
    {
        // If path doesn't exist then try to create it

        snprintf(mkdir_cmd, CC_PATH_LEN, MKDIR " -p %s", path);

        printf("Creating path: %s\n", path);

        if(system(mkdir_cmd) != 0)
        {
            return(EXIT_FAILURE);
        }

        return(EXIT_SUCCESS);
    }

    // Report error if the path is not a directory

    if(!S_ISDIR(path_stat.st_mode))
    {
        ccParsPrintError("output path '%s' is not valid", path);
        return(EXIT_FAILURE);
    }

    return(EXIT_SUCCESS);
}
/*---------------------------------------------------------------------------------------------------------*/
void ccFileRecoverPath(void)
/*---------------------------------------------------------------------------------------------------------*\
  This function will try to recover the initial path where it is written by the CD command.
\*---------------------------------------------------------------------------------------------------------*/
{
    FILE   *f;
    char    cwd_buf[CC_PATH_LEN];

    // Try to open the file with the path

    snprintf(ccfile.cwd_file_path, CC_PATH_LEN, "%s/%s", ccfile.base_path, CC_CWD_FILE);

    f = fopen(ccfile.cwd_file_path, "r");

    if(f == NULL)
    {
        return;
    }

    // Try to read path from start of file - ignore errors

    if(fgets(cwd_buf, CC_PATH_LEN, f) != NULL)
    {
        chdir(cwd_buf);
    }

    fclose(f);
}
/*---------------------------------------------------------------------------------------------------------*/
void ccFileGetBasePath(char *argv0)
/*---------------------------------------------------------------------------------------------------------*\
  This function will try to get the absolute path to the cctest project directory
\*---------------------------------------------------------------------------------------------------------*/
{
    // Get path to cctest executable

    snprintf(ccfile.base_path, CC_PATH_LEN, "%s/../..",dirname(argv0));

    // If not Windows, path may be relative or absolute

#ifndef __MINGW32__
    if(ccfile.base_path[0] != '/')
    {
        char    cwd[CC_PATH_LEN];
        char    base_path[CC_PATH_LEN];

        // Get absolute path to current working directory

        if(getcwd(cwd, sizeof(cwd)) == NULL)
        {
            printf("Fatal - getting current directory : %s (%d)\n", strerror(errno), errno);
            exit(EXIT_FAILURE);
        }

        // Concatenate with relative project path

        snprintf(base_path, CC_PATH_LEN, "%s/%s", cwd, ccfile.base_path );
        strcpy(ccfile.base_path, base_path);
    }
#endif
}
/*---------------------------------------------------------------------------------------------------------*/
FILE * ccFileOpenResultsFile(char *file_type, char *filename, char *file_path)
/*---------------------------------------------------------------------------------------------------------*\
  This function will try to open a CSV, HTML or CCD (CC Debug) file for output
\*---------------------------------------------------------------------------------------------------------*/
{
    FILE *   fp;
    char     path[CC_PATH_LEN];

    snprintf(path, CC_PATH_LEN, "%s/results/%s/%s/%s",
             ccfile.base_path,
             file_type,
             ccpars_global.group,
             ccpars_global.project);

    if(ccFileMakePath(path) == EXIT_FAILURE)
    {
        return(NULL);
    }

    snprintf(file_path, CC_PATH_LEN, "%s/%s.%s", path, filename, file_type);

    fp = fopen(file_path, "w");

    if(fp == NULL)
    {
         ccParsPrintError("opening file '%s' : %s (%d)", file_path, strerror(errno), errno);
         return(NULL);
    }

    return(fp);
}
/*---------------------------------------------------------------------------------------------------------*/
void ccFileWriteCsvNames(struct cclog *log)
/*---------------------------------------------------------------------------------------------------------*\
  This function will write the names for all enabled signals in the log
\*---------------------------------------------------------------------------------------------------------*/
{
    uint32_t sig_idx;

    for(sig_idx = 0 ; sig_idx < log->num_ana_signals ; sig_idx++)
    {
        if(log->ana_sigs[sig_idx].is_enabled)
        {
            fprintf(ccfile.csv_file,",%s", log->ana_sigs[sig_idx].name);
        }
    }

    for(sig_idx = 0 ; sig_idx < log->num_dig_signals ; sig_idx++)
    {
        if(log->dig_sigs[sig_idx].is_enabled)
        {
            fprintf(ccfile.csv_file,",%s", log->dig_sigs[sig_idx].name);
        }
    }
}

/*---------------------------------------------------------------------------------------------------------*/
static void ccFileWriteCsvValueForOneLog(struct cclog *log)
/*---------------------------------------------------------------------------------------------------------*\
  This function will write the values for all enabled signals in the log
\*---------------------------------------------------------------------------------------------------------*/
{
    uint32_t sig_idx;

    for(sig_idx = 0 ; sig_idx < log->num_ana_signals ; sig_idx++)
    {
        if(log->ana_sigs[sig_idx].is_enabled)
        {
            fprintf(ccfile.csv_file,",%.7E",*log->ana_sigs[sig_idx].source);
        }
    }

    for(sig_idx = 0 ; sig_idx < log->num_dig_signals ; sig_idx++)
    {
        if(log->dig_sigs[sig_idx].is_enabled)
        {
            fprintf(ccfile.csv_file,",%u",(uint32_t)*log->dig_sigs[sig_idx].source);
        }
    }
}



/*---------------------------------------------------------------------------------------------------------*/
void ccFileWriteCsvValues(double iter_time)
/*---------------------------------------------------------------------------------------------------------*\
  This function will write one line of CSV values if CSV_OUTPUT is ENABLED.
\*---------------------------------------------------------------------------------------------------------*/
{
    if(ccpars_global.csv_output == REG_ENABLED)
    {
        // Print the iteration time first with microsecond resolution

        fprintf(ccfile.csv_file,"%.6f",iter_time);

        // Print enabled analogue and digital signal values

        ccFileWriteCsvValueForOneLog(&breg_log);
        ccFileWriteCsvValueForOneLog(&ireg_log);
        ccFileWriteCsvValueForOneLog(&meas_log);

         fputc('\n',ccfile.csv_file);
     }
 }

// EOF
