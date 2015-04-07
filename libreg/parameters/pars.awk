#!/usr/bin/awk -f
#
# pars.awk
# Converter Control Regulation library parameter header file generator
#
# All libreg parameters are defined in libreg/parameteres/pars.csv and this
# script translates the csv file into a header file that declares and defines
# structures that describe all the parameters, and a second header file that
# defines a function to initialise the structure.
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
# This file is part of libreg.
#
# libreg is free software: you can redistribute it and/or modify it under the
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
#
# Notes
#
# All libreg parameters are addressed by pointers, so initialization of values
# cannot be done by using the linker. Instead, this script creates the static
# function regMgrParsInit() in the header file libreg_pars_init.h.  This initializes
# the reg_mgr->pars and reg_mgr->pars_values structures. This is called by regMgrInit().
#
# There is a restriction in the current implementation which means that parameters
# that are arrays can only be initialized with the same value in all elements.

BEGIN {

# Set field separater to comma to read csv file

    FS = ","

# Identify the leading columns in the csv file

    pars_column           = 1
    name_column           = 2
    type_column           = 3
    length_column         = 4
    initial_value_column  = 5
    flags_column          = 6
    flag_test_column      = 6
    flag_mode_none_column = 7

# Read heading line from stdin

    getline

    n_columns = NF
    n_pars    = 0
    n_groups  = 0

# Create FLAG_LOAD_SELECT as the first flag

    n_flags = 1
    flag[0] = "FLAG_LOAD_SELECT"

# Create the rest of the flags from the column headings

    for(i=flags_column ; i <= n_columns && index($i,"FLAG_") == 1; i++)
    {
        flag[n_flags++] = $i
    }

    if(n_flags > 32)
    {
        printf "ERROR: Number of flags (%d) exceeds limit (32)\n", n_flags
        exit -1
    }

# Create all the groups from the column headings

    groups_column = i

    for(; i <= n_columns && index($i,"GROUP_") == 1; i++)
    {
        group[n_groups++] = $i
    }

    if(n_groups > 32)
    {
        printf "ERROR: Number of groups (%d) exceeds limit (32)\n", n_groups
        exit -1
    }

# Read parameter definitions from stdin

    while(getline > 0)
    {
        # Skip blank lines

        if($pars_column == "") continue

        # Stop if non-blank lines do not have the correct number of colums

        if($flags_column == "")
        {
            printf "Error in line %d : missing data\n", NR >> "/dev/stderr"
            exit -1
        }

        # Detect if parameter is an array based on LOAD SELECT

        par_length_multiplier[n_pars] = ","

        if($length_column == "REG_NUM_LOADS")
        {
            # Set REG_LOAD_SELECT flag to inform regMgrParsCheck() that this parameter is based on LOAD SELECT

            par_flags[n_pars] = "REG_PAR_" flag[0]

            # For test load select parameters, libreg will keep two values, one for LOAD SELECT and one for LOAD TEST_SELECT
            # if the parameter is not restricted to changes when REG_MODE is NONE.

            if($flag_test_column == "YES" && $flag_mode_none_column == "NO")
            {
                $length_column = "2"
            }
            else
            {
                $length_column = "1"
            }
        }
        else
        {
            par_flags[n_pars] = "0"

            if($length_column != 1)
            {
                par_length_multiplier[n_pars] = "*" $length_column ","
            }
        }

        # Accumulate non-scalar lengths in a hash

        if($length_column != 1)
        {
            par_lengths[$length_column]++
        }

        # Save contents

        par_variable[n_pars] = tolower($pars_column) "_" tolower($name_column)
        par_type    [n_pars] = $type_column
        par_length  [n_pars] = $length_column
        par_init    [n_pars] = $initial_value_column

        # Interpret flag specifiers (YES or NO)
        # Note: flag 0 (FLAG_LOAD_SELECT) is created internally by pars.awk so flags from pars.csv start from index 1

        for(i=1; i < n_flags ; i++)
        {
            f = $(flags_column+i-1)

            if(f == "YES")
            {
                par_flags[n_pars] = par_flags[n_pars] "|REG_PAR_" flag[i]
            }
            else if(f != "NO")
            {
                printf "Error in line %d : column %d (%s) must be YES or NO\n", NR, flags_column+i-1, f >> "/dev/stderr"
                exit -1
            }
        }

        # Interpret group specifiers (YES or NO)

        par_groups[n_pars] = "0"

        for(i=0; i < n_groups ; i++)
        {
            f = $(groups_column+i)

            if(f == "YES")
            {
                par_groups[n_pars] = par_groups[n_pars] "|REG_PAR_" group[i]
            }
            else if(f != "NO")
            {
                printf "Error in line %d : column %d (%s) must be YES or NO\n", NR, groups_column+i, f >> "/dev/stderr"
                exit -1
            }
        }

        n_pars++
    }

# Generate parameter header file inc/libreg_pars.h

    of = "inc/libreg_pars.h"   # Set output file (of)

    print "/*!"                                                                                             > of
    print " * @file  " of                                                                                   > of
    print " * @brief Libreg parameters header file"                                                         > of
    print " *"                                                                                              > of
    print " * IMPORTANT - DO NOT EDIT - This file is generated from libreg/parameters/pars.csv"             > of
    print " *"                                                                                              > of
    print " * All libreg parameters are defined in pars.csv and this is transformed into"                   > of
    print " * this header file by libreg/parameters/pars.awk."                                              > of
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
    print " * This file is part of libreg."                                                                 > of
    print " *"                                                                                              > of
    print " * libreg is free software: you can redistribute it and/or modify it under the"                  > of
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
    print " * To use libreg, the calling program should initialize the parameter value"                     > of
    print " * pointers to access the parameter variables in the calling program. Here"                      > of
    print " * is an example template for all the libreg parameters, which assumes that"                     > of
    print " * the REG_mgr struct is called reg_mgr. Copy this into your initialization"                     > of
    print " * function and for each parameter that you want to control, replace"                            > of
    print " * REG_PAR_NOT_USED with the address of your variable.\n"                                        > of

    for(i=0 ; i < n_pars ; i++)
    {
        printf "    regMgrParInitPointer(&reg_mgr,%-30s,REG_PAR_NOT_USED);\n", par_variable[i]              > of
    }

    print "\n * If a parameter is not used by your program, you can delete the macro"                       > of
    print " * and the default value from pars.csv will be used instead."                                    > of
    print " * If you want a fixed value for a parameter that is different to the"                           > of
    print " * default value in pars.csv, you can set it using another macro:"                               > of
    print " *"                                                                                              > of
    print " *  regMgrParInitValue(&reg_mgr,{parameter_name},{array_index},{initial_value});"                > of
    print " *"                                                                                              > of
    print " * For example, if a given application will always work with the power converter"                > of
    print " * actuation set to REG_CURRENT_REF, then this can be done using:\n"                             > of
    print "    regMgrParInitValue(&reg_mgr,pc_actuation,0,REG_CURRENT_REF);"                                > of
    print " */\n"                                                                                           > of
    print "#ifndef LIBREG_PARS_H"                                                                           > of
    print "#define LIBREG_PARS_H\n"                                                                         > of
    print "#define REG_NUM_PARS                 ", n_pars                                                   > of
    print "#define REG_PAR_NOT_USED              (void*)0\n"                                                > of

    print "#define regMgrParInitPointer(REG_MGR,PAR_NAME,VALUE_P)        (REG_MGR)->pars.u.values.PAR_NAME=VALUE_P"        > of
    print "#define regMgrParInitValue(rEG_MGR,PAR_NAME,INDEX,INIT_VALUE) (REG_MGR)->par_values.PAR_NAME[INDEX]=INIT_VALUE" > of

    print "\n// Parameter flags\n"                                                                          > of

    for(i=0 ; i < n_flags ; i++)
    {
        printf "#define REG_PAR_%-35s(1u<<%d)\n", flag[i], i                                                > of
    }

    print "\n// Parameter groups\n"                                                                         > of

    for(i=0 ; i < n_groups ; i++)
    {
        printf "#define REG_PAR_%-35s(1u<<%d)\n", group[i], i                                               > of
    }

    print "\nstruct REG_pars"                                                                               > of
    print "{"                                                                                               > of
    print "    union"                                                                                       > of
    print "    {"                                                                                           > of
    print "        struct"                                                                                  > of
    print "        {"                                                                                       > of

    for(i=0 ; i < n_pars ; i++)
    {
        printf "           %-25s *%s;\n", par_type[i], par_variable[i]                                      > of
    }

    print "        } values;"                                                                               > of
    print "        void                 *value[REG_NUM_PARS];"                                              > of
    print "    } u;\n"                                                                                      > of

    print "    void                     *copy_of_value[REG_NUM_PARS];\n"                                    > of
    print "    struct REG_pars_meta"                                                                        > of
    print "    {"                                                                                           > of
    print "        uint32_t              size_in_bytes;"                                                    > of
    print "        uint32_t              flags;"                                                            > of
    print "        uint32_t              groups;"                                                           > of
    print "    } meta[REG_NUM_PARS];"                                                                       > of
    print "};\n"                                                                                            > of

    print "struct REG_par_values"                                                                           > of
    print "{"                                                                                               > of

    for(i=0 ; i < n_pars ; i++)
    {
        printf "    %-25s %-30s[%s];\n", par_type[i], par_variable[i], par_length[i]                        > of
    }

    print "};\n"                                                                                            > of

    print "#endif // LIBREG_PARS_H\n"                                                                       > of
    print "// EOF"                                                                                          > of

    close(of)

# Generate parameter initialisation function in inc/init_pars.h

    of = "inc/libreg_pars_init.h"    # Set output file (of)

    print "/*!"                                                                                             > of
    print " * @file  " of                                                                                   > of
    print " * @brief Libreg parameter initialisation header file"                                           > of
    print " *"                                                                                              > of
    print " * IMPORTANT - DO NOT EDIT - This file is generated from libreg/parameters/pars.csv"             > of
    print " *"                                                                                              > of
    print " * All libreg parameters are defined in pars.csv and this is transformed into"                   > of
    print " * this header file (and libreg_pars.h) by libreg/parameters/pars.awk."                          > of
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
    print " * This file is part of libreg."                                                                 > of
    print " *"                                                                                              > of
    print " * libreg is free software: you can redistribute it and/or modify it under the"                  > of
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
    print "#ifndef LIBREG_PARS_INIT_H"                                                                      > of
    print "#define LIBREG_PARS_INIT_H\n"                                                                    > of
    print "static struct REG_pars_meta reg_pars_init_meta[REG_NUM_PARS] = \n{"                              > of

    for(i=0 ; i < n_pars ; i++)
    {
        printf "    {   // %3u. %s\n", i, par_variable[i]                                                   > of
        printf "        sizeof(%s)%s\n", par_type  [i], par_length_multiplier[i]                            > of
        printf "        %s,\n",          par_flags [i]                                                      > of
        printf "        %s,\n",          par_groups[i]                                                      > of
        printf "    },\n"                                                                                   > of
    }

    print "};\n\nstatic void regMgrParsInit(struct REG_mgr *reg_mgr)"                                       > of
    print "{"                                                                                               > of
    print "    uint32_t i;\n"                                                                               > of
    print "    memcpy(reg_mgr->pars.meta,reg_pars_init_meta,sizeof(reg_pars_init_meta));\n"                 > of

    // Initialise pointers to copy of values

    for(i=0 ; i < n_pars ; i++)
    {
        printf "    reg_mgr->pars.copy_of_value[%3u] = &reg_mgr->par_values.%s;\n", i, par_variable[i]      > of
    }

    printf "\n"                                                                                             > of

    // Initialise scalar values

    for(i=0 ; i < n_pars ; i++)
    {
        if(par_length[i] == 1)
        {
            printf "    reg_mgr->par_values.%-30s[0]=%s;\n",par_variable[i], par_init[i]                    > of
        }
    }

    // Initialise vector values

    for(par_len in par_lengths)
    {
        printf "\n    for(i=0;i<%s;i++)\n    {\n", par_len                                                  > of

        for(i=0 ; i < n_pars ; i++)
        {
            if(par_length[i] == par_len)
            {
                printf "        reg_mgr->par_values.%-26s[i]=%s;\n", par_variable[i], par_init[i]           > of
            }

        }
        print "    }"                                                                                       > of
    }

    print "}\n\n#endif // LIBREG_PARS_INIT_H\n"                                                               > of
    print "// EOF"                                                                                          > of
    close(of)

    exit 0
}
# EOF
