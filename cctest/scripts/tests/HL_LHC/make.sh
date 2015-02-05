#!/bin/bash

cd `dirname $0`

source ../../run_header.sh

set +x

rm -rf cctest results
mkdir cctest results

dos2unix -q input/*

awk -f squeeze.awk input/*.csv

# Run CCTEST to simulate squeeze functions

echo -n "Starting cctest in 5s..."
sleep 1
echo -n "4..."
sleep 1
echo -n "3..."
sleep 1
echo -n "2..."
sleep 1
echo -n "1..."
sleep 1

# $cctest "global csv_output $csv_output" "global debug_output $debug_output" "read squeeze.cct"

awk -f chart.awk I_REF_RST     input/*.csv
awk -f chart.awk V_REF_LIMITED input/*.csv

# EOF
