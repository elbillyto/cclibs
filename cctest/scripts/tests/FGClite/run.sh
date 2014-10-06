#!/bin/bash
#
cd `dirname $0`

source ../../run_header.sh

# RST tests

$cctest "global csv_format $csv_format" "read lite.cct"
$cctest "global csv_format $csv_format" "read lite100.cct"

# EOF
