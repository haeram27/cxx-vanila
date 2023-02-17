#!/usr/bin/env bash

## run make with 120% of core jobs for maximum performance
ncores=`cat /proc/cpuinfo | grep cores | wc -l`

# bash
if type bc &>/dev/null; then
    make -j $(printf %.0f `echo "${ncores}*1.2" | bc`) $*
else
    echo error: bc is NOT installed
fi

# zsh
# make -j ${$((${ncores}*1.2))%.*} $*
