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

    pars_column    = 1
    name_column    = 2
    type_column    = 3
    length_column  = 4
    cyc_sel_column = 5
    groups_column  = 6

# Read heading line from stdin

    getline

    n_columns = NF
    n_pars    = 0
    n_groups  = 0

# Create the group masks from the column headings

    for(i=groups_column ; i <= n_columns && index($i,"GROUP_") == 1; i++)
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

        if($groups_column == "")
        {
            printf "Error in line %d : missing data\n", NR >> "/dev/stderr"
            exit -1
        }

        # Save contents

        par_label   [n_pars] = $pars_column " " $name_column
        par_variable[n_pars] = tolower($pars_column) "_" tolower($name_column)
        par_type    [n_pars] = $type_column
        par_length  [n_pars] = $length_column
        par_cyc_sel [n_pars] = $cyc_sel_column

        # Interpret group specifiers (YES or NO)

        par_groups[n_pars] = "0"

        for(i=0; i < n_groups ; i++)
        {
            f = $(groups_column+i)

            if(f == "YES")
            {
                par_groups[n_pars] = par_groups[n_pars] "|REF_PAR_" group[i]
            }
            else if(f != "NO")
            {
                printf "Error in line %d : column %d (%s) must be YES or NO\n", NR, groups_column+i, f >> "/dev/stderr"
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
    print " * the REF_mgr struct is called ref_mgr. Copy this into your initialization"                     > of
    print " * function and for each parameter that you want to control, replace"                            > of
    print " * REF_PAR_NOT_USED with the address of your variable.\n"                                        > of

    for(i=0 ; i < n_pars ; i++)
    {
        printf "    refParInitPointers(&ref_mgr,%-30s,REF_PAR_NOT_USED);\n", par_variable[i] > of
        printf "    refParInitPointers(&ref_mgr,%-30s,REF_PAR_NOT_USED);\n", par_variable[i] > of
    }

    print "\n */\n"                                                                                         > of
    print "#ifndef LIBREF_PARS_H"                                                                           > of
    print "#define LIBREF_PARS_H\n"                                                                         > of
    print "#define REF_NUM_PARS                  ", n_pars                                                  > of
    print "#define REF_PAR_NOT_USED              (void*)0\n"                                                > of

    print "#define refMgrParInitPointer(REF_MGR,PAR_NAME,VALUE_P)         (REF_MGR)->u.pars.PAR_NAME.value=VALUE_P"                  > of
    print "#define refMgrParInitUserData(REF_MGR,PAR_NAME,USER_DATA_P)    (REF_MGR)->u.pars.PAR_NAME.meta.user_data=USER_DATA_P"     > of
    print "#define refMgrParInitCycSelStep(REF_MGR,PAR_NAME,CYC_SEL_STEP) (REF_MGR)->u.pars.PAR_NAME.meta.cyc_sel_step=CYC_SEL_STEP" > of
    print "#define refMgrParInitDevSelStep(REF_MGR,PAR_NAME,DEV_SEL_STEP) (REF_MGR)->u.pars.PAR_NAME.meta.dev_sel_step=DEV_SEL_STEP" > of

    print "\n// Parameter groups\n"                                                                         > of

    for(i=0 ; i < n_groups ; i++)
    {
        printf "#define REF_PAR_%-35s(1u<<%d)\n", group[i], i                                               > of
    }

    print "\n// Parameter structures and union\n"                                                           > of

    print "struct REF_par_meta"                                                                             > of
    print "{"                                                                                               > of
    print "    void                     *user_data;"                                                        > of
    print "    uint32_t                  cyc_sel_step;"                                                     > of
    print "    uint32_t                  dev_sel_step;"                                                     > of
    print "    uint32_t                  max_elements;"                                                     > of
    print "    uint32_t                  size_of_element;"                                                  > of
    print "    uint32_t                  groups;"                                                           > of
    print "};\n"                                                                                            > of
                                                                                                              
    print "struct REF_par"                                                                                  > of
    print "{"                                                                                               > of
    print "    void                       *value;"                                                          > of
    print "    struct REF_par_meta         meta;"                                                           > of
    print "};\n"                                                                                            > of
                                                                                                              
    print "union REF_pars"                                                                                  > of
    print "{"                                                                                               > of
    print "    struct REF_par              par[REF_NUM_PARS];"                                              > of
    print "    struct"                                                                                      > of
    print "    {"                                                                                           > of
                                                                                                              
    for(i=0 ; i < n_pars ; i++)                                                                               
    {                                                                                                         
        printf "        struct  // %3u. %s\n", i, par_label[i]                                              > of
        print  "        {"                                                                                  > of
        printf "            %-25s *value;\n", par_type[i]                                                   > of
        print  "            struct REF_par_meta        meta;"                                               > of
        printf "        } %s;\n\n", par_variable[i]                                                         > of
    }                                                                                                         
                                                                                                              
    print "    } pars;"                                                                                     > of
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

    print "static void refParsInit(struct REF_mgr *ref_mgr)"                                                > of
    print "{"                                                                                               > of

    for(i=0 ; i < n_pars ; i++)
    {
        printf   "    ref_mgr->u.pars.%s.meta.max_elements = %s;\n", par_variable[i], par_length[i]          > of
        printf   "    ref_mgr->u.pars.%s.meta.size_of_element = sizeof(%s);\n", par_variable[i], par_type[i] > of
        printf   "    ref_mgr->u.pars.%s.meta.groups = %s;\n\n", par_variable[i], par_groups[i]               > of
    }

    print "}\n"                                                                                             > of
    print "#endif // LIBREF_INIT_PARS_H\n"                                                                  > of
    print "// EOF"                                                                                          > of
    close(of)

    exit 0
}
# EOF
