#!/usr/bin/env bash 

##############
# VARS
##############
IMODE="imod.main"


##############
# VAR COMMON
##############
TPTNORM=$(tput sgr0)
TPTDBLACK=$(tput setaf 0 && tput bold)
TPTDRED=$(tput setaf 1 && tput bold)
TPTDGREEN=$(tput setaf 2 && tput bold)
TPTDYELLOW=$(tput setaf 3 && tput bold)
TPTDBLUE=$(tput setaf 4 && tput bold)
TPTDPURPLE=$(tput setaf 5 && tput bold)
TPTDCYAN=$(tput setaf 6 && tput bold)
TPTDWHITE=$(tput setaf 7 && tput bold)
TPTBLACK=$(tput setaf 8 && tput bold)
TPTRED=$(tput setaf 9 && tput bold)
TPTGREEN=$(tput setaf 10 && tput bold)
TPTYELLOW=$(tput setaf 11 && tput bold)
TPTBLUE=$(tput setaf 12 && tput bold)
TPTPURPLE=$(tput setaf 13 && tput bold)
TPTCYAN=$(tput setaf 14 && tput bold)
TPTWHITE=$(tput setaf 15 && tput bold)


##############
# FUNCS
##############

# main
func.main() {
    if ! type make &>/dev/null; then
        echo ${TPTRED}errori${TPTNORM}: can NOT find make
        exit 1;
    fi

    make -pn | grep -A1 "^# makefile"| grep -v "^#\|^--" | sort | uniq
}


# gettime
func.printall() {
    if ! type make &>/dev/null; then
        echo ${TPTRED}errori${TPTNORM}: can NOT find make
        exit 1;
    fi

    make -pn | grep -v "^#\|^--" | sort | uniq
}

# help
func.help() {
cat <<HELP
$(basename $0) - Print rule and variable in makefile
Usage: $(basename $0) [options...]
      -h    help.
      -a    print including pre-defined rule and variable of make
HELP
}


##############
# INIT
##############
while getopts "ha\?" opt
do
    case ${opt} in
    h|\?) IMODE="imod.help" ;;
    a) IMODE="imod.all" ;;
    esac
done

#echo "IMODE=${IMODE}"
case $IMODE in
    "imod.help") func.help ;;
    "imod.main") func.main ;;
    "imod.all") func.printall;;
    *) exit 1 ;;
esac

exit 0
