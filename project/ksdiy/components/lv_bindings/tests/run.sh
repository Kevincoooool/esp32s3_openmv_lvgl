#!/bin/bash

set -e # Exit on error

SCRIPT_PATH="`dirname \"$0\"`"

TEST_PATH="$SCRIPT_PATH/../examples/*.py $SCRIPT_PATH/../lvgl/examples/"
EXCLUDE_PATH="$SCRIPT_PATH/../examples/fb_test.py $SCRIPT_PATH/../examples/uasyncio*.py $SCRIPT_PATH/../examples/advanced_demo.py"

EXCLUDE_FINDEXP=$(echo $EXCLUDE_PATH | sed "s/^\|[[:space:]]/ -and -not -path /g")

find $TEST_PATH -name "*.py" $EXCLUDE_FINDEXP |\
   xargs -I {} timeout 5m catchsegv $SCRIPT_PATH/../../../ports/unix/micropython-dev $SCRIPT_PATH/run_test.py {}

