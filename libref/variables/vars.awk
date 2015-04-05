#!/usr/bin/awk -f
#
# vars.awk
# Converter Control Reference Manager library read-only variables header file generator
#
# All libref variables that might be interesting to an application are
# identified in vars.csv. This allows this script to create a
# header file with a macro and constants that allow the application developer
# easy read-only access the variables.
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

# Identify the columns in the csv file

    var_group_column     = 1
    var_id_column        = 2
    var_type_column      = 3
    var_name_column      = 4
    var_comment_column   = 5

# Read heading line from stdin

    getline

# Read variable definitions from stdin

    n_vars = 0

    while(getline > 0)
    {
        # Skip blank lines

        if($var_group_column == "") continue

        # Stop if non-blank lines do not have the correct number of colums

        if($var_comment_column == "")
        {
            printf "Error in line %d : missing data\n", NR >> "/dev/stderr"
            exit -1
        }

        # Save contents

        var_id      [n_vars] = $var_group_column "_" $var_id_column
        var_type    [n_vars] = $var_type_column
        var_name    [n_vars] = $var_name_column
        var_comment [n_vars] = $var_comment_column

        n_vars++
    }

# Generate variable header file inc/libref_vars.h

    of = "inc/libref_vars.h"   # Set output file (of)

    print "/*!"                                                                                                      > of
    print " * @file  " of                                                                                            > of
    print " * @brief Converter Control Reference Manager library generated read-only variables header file"                 > of
    print " *"                                                                                                       > of
    print " * IMPORTANT - DO NOT EDIT - This file is generated from libref/variables/vars.csv"                       > of
    print " *"                                                                                                       > of
    print " * All libref read-only variables are defined in vars.csv and this is transformed into"                   > of
    print " * this header file by libref/variables/vars.awk."                                                        > of
    print " *"                                                                                                       > of
    print " * <h2>Contact</h2>"                                                                                      > of
    print " *"                                                                                                       > of
    print " * cclibs-devs@cern.ch"                                                                                   > of
    print " *"                                                                                                       > of
    print " * <h2>Copyright</h2>"                                                                                    > of
    print " *"                                                                                                       > of
    print " * Copyright CERN 2014. This project is released under the GNU Lesser General"                            > of
    print " * Public License version 3."                                                                             > of
    print " *"                                                                                                       > of
    print " * <h2>License</h2>"                                                                                      > of
    print " *"                                                                                                       > of
    print " * This file is part of libref."                                                                          > of
    print " *"                                                                                                       > of
    print " * libref is free software: you can redistribute it and/or modify it under the"                           > of
    print " * terms of the GNU Lesser General Public License as published by the Free"                               > of
    print " * Software Foundation, either version 3 of the License, or (at your option)"                             > of
    print " * any later version."                                                                                    > of
    print " *"                                                                                                       > of
    print " * This program is distributed in the hope that it will be useful, but WITHOUT"                           > of
    print " * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or"                                 > of
    print " * FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License"                           > of
    print " * for more details."                                                                                     > of
    print " *"                                                                                                       > of
    print " * You should have received a copy of the GNU Lesser General Public License"                              > of
    print " * along with this program.  If not, see <http://www.gnu.org/licenses/>."                                 > of
    print " *"                                                                                                       > of
    print " */\n"                                                                                                    > of
    print "#ifndef LIBREF_VARS_H"                                                                                    > of
    print "#define LIBREF_VARS_H\n"                                                                                  > of
    print "#include <stdint.h>"                                                                                      > of
    print "#include <stdbool.h>\n"                                                                                   > of
    print "#define refMgrVar(ref_mgr, var_key)  (((struct REF_mgr const *)&ref_mgr)->var_key)\n"                     > of

    for(i=0 ; i < n_vars ; i++)
    {
        printf "#define %-30s %-40s // %-15s %s\n",var_id[i], var_name[i], var_type[i], var_comment[i]               > of
    }

    print "\n#endif // LIBREF_VARS_H\n"                                                                              > of
    print "// EOF"                                                                                                   > of

    close(of)

# Generate test file inc/libref_vars_test.h

    of = "inc/libref_vars_test.h"   # Set output file (of)

    print "/*!"                                                                                                      > of
    print " * @file  " of                                                                                            > of
    print " * @brief Converter Control Reference Manager library generated read-only variables macros test file"            > of
    print " *"                                                                                                       > of
    print " * IMPORTANT - DO NOT EDIT - This file is generated from libref/variables/vars.csv"                       > of
    print " *"                                                                                                       > of
    print " * All libref read-only variables are defined in vars.csv and this is transformed into"                   > of
    print " * the inc/libref_vars.h header file and this test file by libref/variables/vars.awk."                    > of
    print " *"                                                                                                       > of
    print " * <h2>Contact</h2>"                                                                                      > of
    print " *"                                                                                                       > of
    print " * cclibs-devs@cern.ch"                                                                                   > of
    print " *"                                                                                                       > of
    print " * <h2>Copyright</h2>"                                                                                    > of
    print " *"                                                                                                       > of
    print " * Copyright CERN 2014. This project is released under the GNU Lesser General"                            > of
    print " * Public License version 3."                                                                             > of
    print " *"                                                                                                       > of
    print " * <h2>License</h2>"                                                                                      > of
    print " *"                                                                                                       > of
    print " * This file is part of libref."                                                                          > of
    print " *"                                                                                                       > of
    print " * libref is free software: you can redistribute it and/or modify it under the"                           > of
    print " * terms of the GNU Lesser General Public License as published by the Free"                               > of
    print " * Software Foundation, either version 3 of the License, or (at your option)"                             > of
    print " * any later version."                                                                                    > of
    print " *"                                                                                                       > of
    print " * This program is distributed in the hope that it will be useful, but WITHOUT"                           > of
    print " * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or"                                 > of
    print " * FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License"                           > of
    print " * for more details."                                                                                     > of
    print " *"                                                                                                       > of
    print " * You should have received a copy of the GNU Lesser General Public License"                              > of
    print " * along with this program.  If not, see <http://www.gnu.org/licenses/>."                                 > of
    print " *"                                                                                                       > of
    print " */\n"                                                                                                    > of
    print "#include <libref_vars.h>\n"                                                                               > of
    print "static void refMgrTestVarMacros(void)"                                                                    > of
    print "{"                                                                                                        > of
    print "    double accumulator = 0.0;     // This is used to suppress unused variable warnings"                   > of
    print "    struct REF_mgr ref_mgr;\n"                                                                            > of

    for(i=0 ; i < n_vars ; i++)
    {
        printf "    %-23s %-30s = refMgrVar(ref_mgr,%s);\n", var_type[i], tolower(var_id[i]), var_id[i]              > of
        printf "    accumulator += %s;\n", tolower(var_id[i])                                                        > of
    }

    print "}\n\n// EOF"                                                                                              > of

    close(of)

    exit 0



}
# EOF
