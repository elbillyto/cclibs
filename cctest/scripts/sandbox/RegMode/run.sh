#!/bin/bash
#
cd `dirname $0`

source ../../run_header.sh

# Reg Mode tests

$cctest "global csv_format $csv_format" "read regmode.cct"

# EOF
