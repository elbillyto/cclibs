#!/bin/bash
#
cd `dirname $0`

source ../../run_header.sh

# Reverse time function generation tests

$cctest "global csv_output $csv_output" "global debug_output $debug_output" "read reverse.cct"

>&2 echo $0 complete

# EOF
