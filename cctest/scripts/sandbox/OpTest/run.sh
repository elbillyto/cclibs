#!/bin/bash
#
cd `dirname $0`

source ../../run_header.sh

# Operation and test parameter switching tests

$cctest "global csv_output $csv_output" "global debug_output $debug_output" "read optest.cct"

>&2 echo $0 complete

# EOF
