#
# Script used in all run.sh scripts
#
if [ $# -eq 0 ]; then
  csv_output=DISABLED
  debug_output=DISABLED
elif [ $# -eq 1 ]; then
  csv_output=$1
  debug_output=DISABLED
elif [ $# -eq 2 ]; then
  csv_output=$1
  debug_output=$2
else
  echo usage: run.sh [CSV_OUTPUT [DEBUG_OUTPUT]] 
  exit -1
fi

cctest=../../../`uname -s`/`uname -m`/cctest

set -x
# EOF
