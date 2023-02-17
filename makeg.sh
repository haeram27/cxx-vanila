#!/usr/bin/env bash

## main$ ./makeg  :: empty parameter prints list of test
## main$ ./makeg VanilaTest.test1
## main$ ./makeg "VanilaTest.*"
## main$ ./makeg VanilaTest.\*

ncores=`cat /proc/cpuinfo | grep cores | wc -l`
if type bc &>/dev/null; then
    if [[ -z $1 ]]; then
        make -j $(printf %.0f `echo "${ncores}*1.2" | bc`) gtest.list
    else 
        make -j $(printf %.0f `echo "${ncores}*1.2" | bc`) gtest.run $*
    fi
else
    echo error: bc is NOT installed
fi
