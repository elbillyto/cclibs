#!/bin/bash
#
cd `dirname $0`

source ../../run_header.sh

# TEST function (SINE, COSINE, SQUARE, STEPS) tests

$cctest "global csv_format $csv_format" "read test.cct"

>&2 echo $0 complete

# EOF
