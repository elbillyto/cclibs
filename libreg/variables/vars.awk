#!/usr/bin/awk -f
#
# vars.awk
# Converter Control Regulation library read-only variables header file generator
#
# All libreg variables that might be interesting to an application are
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

BEGIN {

# Set field separater to comma to read csv file

    FS = ","

# Identify the columns in the csv file

    pars_column        = 1
    name_column        = 2
    type_column        = 3
    reg_mgr_var_column = 4
    comment_column     = 5

# Read heading line from stdin

    getline

# Read variable definitions from stdin

    n_vars = 0

    while(getline > 0)
    {
        # Skip blank lines

        if($pars_column == "") continue

        # Stop if non-blank lines do not have the correct number of colums

        if($comment_column == "")
        {
            printf "Error in line %d : missing data\n", NR >> "/dev/stderr"
            exit -1
        }

        # Save contents

        var_id     [n_vars] = $pars_column "_" $name_column
        var_type   [n_vars] = $type_column
        var_reg_mgr[n_vars] = $reg_mgr_var_column
        var_comment[n_vars] = $comment_column

        n_vars++
    }

# Generate variable header file inc/libreg_vars.h

    of = "inc/libreg_vars.h"   # Set output file (of)

    print "/*!"                                                                                                      > of
    print " * @file  " of                                                                                            > of
    print " * @brief Converter Control Regulation library generated read-only variables header file"                 > of
    print " *"                                                                                                       > of
    print " * IMPORTANT - DO NOT EDIT - This file is generated from libreg/variables/vars.csv"                       > of
    print " *"                                                                                                       > of
    print " * All libreg read-only variables are defined in vars.csv and this is transformed into"                   > of
    print " * this header file by libreg/variables/vars.awk."                                                        > of
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
    print " * This file is part of libreg."                                                                          > of
    print " *"                                                                                                       > of
    print " * libreg is free software: you can redistribute it and/or modify it under the"                           > of
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
    print "#ifndef LIBREG_VARS_H"                                                                                    > of
    print "#define LIBREG_VARS_H\n"                                                                                  > of

    print "/*!"                                                                                                      > of
    print " * Use regMgrVar() with the reg_mgr structure. It is possible to take the address of regMgrVar() when"    > of
    print " * initialising a pointer - i.e. the linker can resolve the address."                                     > of
    print " *"                                                                                                       > of
    print " * Use regMgrVarP() with a pointer to the reg_mgr structure. It is NOT possible to take the address of"   > of
    print " * regMgrVarP() when initialising a pointer - i.e. the linker CANNOT resolve the address."                > of
    print " *"                                                                                                       > of
    print " */\n"                                                                                                    > of

    print "#define regMgrVar(REG_MGR, VAR_KEY)  (((struct REG_mgr const *)&REG_MGR)->REG_VAR_ ## VAR_KEY)"           > of
    print "#define regMgrVarP(REG_MGR_P, VAR_KEY)  (REG_MGR_P->REG_VAR_ ## VAR_KEY)\n"                               > of

    for(i=0 ; i < n_vars ; i++)
    {
        printf "#define REG_VAR_%-30s %-40s // %-15s %s\n",var_id[i], var_reg_mgr[i], var_type[i], var_comment[i]    > of
    }

    print "\n#endif // LIBREG_VARS_H\n"                                                                              > of
    print "// EOF"                                                                                                   > of

    close(of)

# Generate test file inc/libreg_vars_test.h

    of = "inc/libreg_vars_test.h"   # Set output file (of)

    print "/*!"                                                                                                      > of
    print " * @file  " of                                                                                            > of
    print " * @brief Converter Control Regulation library generated read-only variables macros test file"            > of
    print " *"                                                                                                       > of
    print " * IMPORTANT - DO NOT EDIT - This file is generated from libreg/variables/vars.csv"                       > of
    print " *"                                                                                                       > of
    print " * All libreg read-only variables are defined in vars.csv and this is transformed into"                   > of
    print " * the inc/libreg_vars.h header file and this test file by libreg/variables/vars.awk."                    > of
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
    print " * This file is part of libreg."                                                                          > of
    print " *"                                                                                                       > of
    print " * libreg is free software: you can redistribute it and/or modify it under the"                           > of
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
    print "#include <libreg_vars.h>\n"                                                                               > of
    print "static void regMgrTestVarMacros(void)"                                                                    > of
    print "{"                                                                                                        > of
    print "    double accumulator = 0.0;     // This is used to suppress unused variable warnings"                   > of
    print "    struct REG_mgr reg_mgr;\n"                                                                            > of

    for(i=0 ; i < n_vars ; i++)
    {
        printf "    %-23s %-30s = regMgrVar(reg_mgr,%s);\n", var_type[i], tolower(var_id[i]), var_id[i]              > of
        printf "    accumulator += %s;\n", tolower(var_id[i])                                                        > of
    }

    print "}\n\n// EOF"                                                                                              > of

    close(of)

    exit 0
}
# EOF
