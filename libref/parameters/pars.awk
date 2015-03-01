#!/usr/bin/awk -f
#
# pars.awk
# Converter Control Reference management library parameter header file generator
#
# All libref parameters are defined in libref/parameteres/pars.csv and this
# script translates the csv file into a header file that declares and defines
# structures that describe all the parameters.
#
# Contact
#
# cclibs-devs@cern.ch
#
# Copyright
#
# Copyright CERN 2014. This project is released under the GNU Lesser General
# Public License version 3.
#
# License
#
# This file is part of libref.
#
# libref is free software: you can redistribute it and/or modify it under the
# terms of the GNU Lesser General Public License as published by the Free
# Software Foundation, either version 3 of the License, or (at your option)
# any later version.
#
# This program is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
# FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License
# for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

BEGIN {

# Set field separater to comma to read csv file

    FS = ","

# Identify the leading columns in the csv file

    par_group_column     = 1
    par_name_column      = 2
    par_type_column      = 3
    par_length_column    = 4
    par_cyc_sel_column   = 5
    par_flags_column     = 6

# Read heading line from stdin

    getline

    n_columns = NF
    n_pars    = 0
    n_flags   = 0

# Create the flags from the column headings

    for(i=par_flags_column ; i <= n_columns ; i++)
    {
        flag[n_flags++] = $i
    }

    if(n_flags > 32)
    {
        printf "ERROR: Number of flags (%d) exceeds limit (32)\n", n_flags
        exit -1
    }

# Read parameter definitions from stdin

    while(getline > 0)
    {
        # Skip blank lines

        if($par_group_column == "") continue

        # Stop if non-blank lines do not have the correct number of colums

        if($par_flags_column == "")
        {
            printf "Error in line %d : missing data\n", NR >> "/dev/stderr"
            exit -1
        }

        # Detect if parameter is an array based on LOAD SELECT

        if($par_length_column == "REF_NUM_LOADS")
        {
            # Set REF_LOAD_SELECT flag to inform refConvParsCheck() that this parameter is based on LOAD SELECT

            par_flags[n_pars] = flag[0]

            # For test load select parameters, libref will keep two values, one for LOAD SELECT and one for LOAD TEST_SELECT

            if($par_test_column == "YES" && $par_mode_none_column == "NO")
            {
                $par_length_column = "2"
            }
            else
            {
                $par_length_column = "1"
            }
        }
        else
        {
            par_flags[n_pars] = "0"
        }

        # Save contents

        par_variable[n_pars] = tolower($par_group_column) "_" tolower($par_name_column)
        par_type    [n_pars] = $par_type_column
        par_length  [n_pars] = $par_length_column
        par_cyc_sel [n_pars] = $par_cyc_sel_column

        # Interpret flag specifiers (YES or NO)

        for(i=0; i < n_flags ; i++)
        {
            f = $(par_flags_column+i)

            if(f == "YES")
            {
                par_flags[n_pars] = par_flags[n_pars] "|" flag[i]
            }
            else if(f != "NO")
            {
                printf "Error in line %d : column %d (%s) must be YES or NO\n", NR, par_flags_column+i, f >> "/dev/stderr"
                exit -1
            }
        }

        n_pars++
    }

# Generate parameter header file inc/libref_pars.h

    of = "inc/libref_pars.h"   # Set output file (of)

    print "/*!"                                                                                             > of
    print " * @file  libref_pars.h"                                                                         > of
    print " * @brief Converter Control Reference management library generated parameters header file"       > of
    print " *"                                                                                              > of
    print " * IMPORTANT - DO NOT EDIT - This file is generated from libref/parameters/pars.csv"             > of
    print " *"                                                                                              > of
    print " * All libref parameters are defined in read_write_pars.csv and this is transformed into"        > of
    print " * this header file by libref/parameters/read_write_pars.awk."                                   > of
    print " *"                                                                                              > of
    print " * <h2>Contact</h2>"                                                                             > of
    print " *"                                                                                              > of
    print " * cclibs-devs@cern.ch"                                                                          > of
    print " *"                                                                                              > of
    print " * <h2>Copyright</h2>"                                                                           > of
    print " *"                                                                                              > of
    print " * Copyright CERN 2014. This project is released under the GNU Lesser General"                   > of
    print " * Public License version 3."                                                                    > of
    print " *"                                                                                              > of
    print " * <h2>License</h2>"                                                                             > of
    print " *"                                                                                              > of
    print " * This file is part of libref."                                                                 > of
    print " *"                                                                                              > of
    print " * libref is free software: you can redistribute it and/or modify it under the"                  > of
    print " * terms of the GNU Lesser General Public License as published by the Free"                      > of
    print " * Software Foundation, either version 3 of the License, or (at your option)"                    > of
    print " * any later version."                                                                           > of
    print " *"                                                                                              > of
    print " * This program is distributed in the hope that it will be useful, but WITHOUT"                  > of
    print " * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or"                        > of
    print " * FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License"                  > of
    print " * for more details."                                                                            > of
    print " *"                                                                                              > of
    print " * You should have received a copy of the GNU Lesser General Public License"                     > of
    print " * along with this program.  If not, see <http://www.gnu.org/licenses/>."                        > of
    print " *"                                                                                              > of
    print " * <h2>Parameter initialization template</h2>"                                                   > of
    print " *"                                                                                              > of
    print " * To use libref, the calling program should initialize the parameter value"                     > of
    print " * pointers to access the parameter variables in the calling program. Here"                      > of
    print " * is an example template for all the libref parameters, which assumes that"                     > of
    print " * the ref_conv struct is called conv. Copy this into your initialization"                       > of
    print " * function and for each parameter that you want to control, replace"                            > of
    print " * REF_PAR_NOT_USED with the address of your variable.\n"                                        > of

    for(i=0 ; i < n_pars ; i++)
    {
        printf "    refParInitPointers(&ref_mgr,%-30s,REF_PAR_NOT_USED,REF_PAR_NOT_USED);\n", par_variable[i] > of
    }

    print "\n */\n"                                                                                         > of
    print "#ifndef LIBREF_PARS_H"                                                                           > of
    print "#define LIBREF_PARS_H\n"                                                                         > of
    print "#define REF_NUM_PARS                  ", n_pars                                                  > of
    print "#define REF_PAR_NOT_USED              (void*)0\n"                                                > of

    print "#define refParInitPointers(ref_mgr,par_name,value_p,num_els_p) (ref_mgr)->pars.par_name.value=value_p,(ref_mgr)->pars.par_name.num_els=num_els_p\n" > of

    for(i=0 ; i < n_flags ; i++)
    {
        printf "#define %-40s(1u<<%d)\n", flag[i], i                                                        > of
    }

    print "\nstruct ref_par"                                                                                > of
    print "{"                                                                                               > of
    print "    void                     *value;"                                                            > of
    print "    uint32_t                 *num_els;"                                                          > of
    print "    uint32_t                  size_in_bytes;"                                                    > of
    print "    uint32_t                  sizeof_type;"                                                      > of
    print "    uint32_t                  flags;"                                                            > of
    print "};\n"                                                                                            > of
    print "struct ref_pars"                                                                                 > of
    print "{"                                                                                               > of

    for(i=0 ; i < n_pars ; i++)
    {
        printf "    struct ref_par            %s;\n", par_variable[i]                                       > of
    }

    print "};\n"                                                                                            > of
    print "#endif // LIBREF_PARS_H\n"                                                                       > of
    print "// EOF"                                                                                          > of

    close(of)

# Generate parameter initialisation function in inc/init_pars.h

    of = "inc/libref_init_pars.h"    # Set output file (of) to init_pars.h

    print "/*!"                                                                                             > of
    print " * @file  libref_init_pars.h"                                                                    > of
    print " * @brief Libref generated parameter initialisation header file"                                 > of
    print " *"                                                                                              > of
    print " * IMPORTANT - DO NOT EDIT - This file is generated from libref/parameters/pars.csv"             > of
    print " *"                                                                                              > of
    print " * All libref parameters are defined in pars.csv and this is transformed into"                   > of
    print " * this header file (and pars.h) by libref/parameters/pars.awk."                                 > of
    print " *"                                                                                              > of
    print " * <h2>Contact</h2>"                                                                             > of
    print " *"                                                                                              > of
    print " * cclibs-devs@cern.ch"                                                                          > of
    print " *"                                                                                              > of
    print " * <h2>Copyright</h2>"                                                                           > of
    print " *"                                                                                              > of
    print " * Copyright CERN 2014. This project is released under the GNU Lesser General"                   > of
    print " * Public License version 3."                                                                    > of
    print " *"                                                                                              > of
    print " * <h2>License</h2>"                                                                             > of
    print " *"                                                                                              > of
    print " * This file is part of libref."                                                                 > of
    print " *"                                                                                              > of
    print " * libref is free software: you can redistribute it and/or modify it under the"                  > of
    print " * terms of the GNU Lesser General Public License as published by the Free"                      > of
    print " * Software Foundation, either version 3 of the License, or (at your option)"                    > of
    print " * any later version."                                                                           > of
    print " *"                                                                                              > of
    print " * This program is distributed in the hope that it will be useful, but WITHOUT"                  > of
    print " * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or"                        > of
    print " * FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License"                  > of
    print " * for more details."                                                                            > of
    print " *"                                                                                              > of
    print " * You should have received a copy of the GNU Lesser General Public License"                     > of
    print " * along with this program.  If not, see <http://www.gnu.org/licenses/>."                        > of
    print " */\n"                                                                                           > of
    print "#ifndef LIBREF_INIT_PARS_H"                                                                      > of
    print "#define LIBREF_INIT_PARS_H\n"                                                                    > of

    print "static void refParsInit(struct ref_mgr *ref_mgr)"                                                > of
    print "{"                                                                                               > of

    for(i=0 ; i < n_pars ; i++)
    {
        printf   "    ref_mgr->pars.%s.size_in_bytes = sizeof(ref_mgr->par_values.%s);\n", par_variable[i], par_variable[i]    > of
        printf   "    ref_mgr->pars.%s.sizeof_type   = sizeof(%s);\n", par_variable[i], par_type[i]                         > of
        printf   "    ref_mgr->pars.%s.flags         = %s;\n", par_variable[i], par_flags[i]                                > of
    }

    print "}\n"                                                                                             > of
    print "#endif // LIBREF_INIT_PARS_H\n"                                                                  > of
    print "// EOF"                                                                                          > of
    close(of)

    exit 0
}
# EOF
