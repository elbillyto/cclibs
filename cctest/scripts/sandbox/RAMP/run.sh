#!/bin/bash
#
cd `dirname $0`

source ../../run_header.sh

# RST tests

$cctest "global csv_output $csv_output" "global debug_output $debug_output" "read ramp.cct"

>&2 echo $0 complete

# EOF
