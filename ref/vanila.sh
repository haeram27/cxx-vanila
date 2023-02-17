#!/usr/bin/env bash 

##############
# VARS
##############
IMODE="imod.main"
VMODE_TIMEUTC="no"


# VARS COMMON {{{
# echo color message:: (recommended) support all type shell
# echo ${TPTRED}----------------------------------
# echo ${TPTRED}warning${TPTNORM}: something wrong
# echo ${TPTRED}----------------------------------
# echo 123GREEN123 | sed -u -e "s/\(GREEN\)/${TPTGREEN}\1${TPTNORM}/gi"
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

# *** make random string
# RANDSTR64=$(cat /dev/urandom | tr -dc 'a-zA-Z0-9' | fold -w 64 | sed 1q)
# RANDSTRDATE64=$(cat /dev/urandom | tr -dc 'a-zA-Z0-9' | fold -w 40 | sed 1q)$(date -u +%Y%m%d%H%M%S%N)
# RANDSTRDATE56=$(uuidgen | tr -d '-')$(date -u +%Y%m%d%H%M%S%N)

# *** ENVENCKEY: $ base64 mysecretkey
# ENCKEY=$(base64 -d <<< ${ENVENCKEY})
# *** ENVENCITR: $ base64 1234
# ENCITR=$(base64 -d <<< ${ENVENCITR})
# *** ENCSECRET
# *** openssl enc -k ${ENCKEY} -aes-256-cbc -a -md sha512 -pbkdf2 -iter ${ENCITR} -salt <<< text.plain
# *** openssl enc -k ${ENCKEY} -aes-256-cbc -a -md sha512 -pbkdf2 -iter ${ENCITR} -salt -in file.plain -out file.enc
# }}}


##############
# FUNCS
##############

# main
func.main() {
    read -p "Do you want Hello World? Are you sure? (y/n) " -n 1;
    echo ""
    if [[ $REPLY =~ ^[Yy]$ ]]; then
        echo ${TPTRED}Hello World${TPTNORM}
        echo ${TPTGREEN}Hello World${TPTNORM}
        echo ${TPTYELLOW}Hello World${TPTNORM}
        echo ${TPTBLUE}Hello World${TPTNORM}
        echo ${TPTPURPLE}Hello World${TPTNORM}
        echo ${TPTCYAN}Hello World${TPTNORM}
        echo ${TPTWHITE}Hello World${TPTNORM}
    else
        echo $REPLY
    fi
}


# gettime
func.gettime() {
cat<<-EOF
gettime()
EOF
    ## override VMODE_TIMEUTC only in function
    #local VMODE_TIMEUTC="yes"

    if [[ "${VMODE_TIMEUTC}" == "yes" ]]; then
        echo 'UTC::'
        while :; do date -u --rfc-3339=ns; sleep 0.1; done;
    else
        echo 'LOCAL::'
        while :; do date --rfc-3339=ns; sleep 0.1; done;
    fi
}


func.rest.request() {
    RESPFILE=\/tmp\/$(uuidgen | tr -d '-')$(date -u +%Y%m%d%H%M%S%N)
    REQTIMEOUT=2
    if [[ -z $1 ]]; then
        REQURI='http://www.google.com'
    else
        REQURI=$1
    fi
   
    RESPCODE=$(curl -fksSL --connect-timeout ${REQTIMEOUT} -w "%{response_code}" -o ${RESPFILE} ${REQURI})
    CURLRET=$?
    if [[ ${CURLRET} -eq 0 ]]; then
        if [[ "${RESPCODE}" == "200" ]]; then
            ## do something with ${RESPFILE}
            echo ${RESPFILE}; cat ${RESPFILE};
        else
            echo ${TPTRED}warning${TPTNORM}: response code is ${TPTRED}${RESPCODE}${TPTNORM} >&2
        fi
    else
        echo ${TPTRED}error${TPTNORM}: curl returns ${TPTRED}${CURLRET}${TPTNORM} >&2
    fi

    [[ -f ${RESPFILE} ]] && (rm -f ${RESPFILE} &>/dev/null)
}


# help
func.help() {
cat <<HELP
$(basename $0) - Print rule and variable in makefile
Usage: $(basename $0) [options...]
    -h    help.
    -q    rest rquest.
    -t    time.
    -u    set utc time when using with -t.
HELP
}


##############
# INIT
##############
while getopts "thuq\?" opt
do
    case ${opt} in
    h|\?) IMODE="imod.help" ;;
    t) IMODE="imod.time" ;;
    q) IMODE="imod.rest" ;;
    u) echo "turn on UTC mode"
        VMODE_TIMEUTC="yes" ;;
    esac
done

#echo "IMODE=${IMODE}"
case $IMODE in
    "imod.help") func.help ;;
    "imod.main") func.main ;;
    "imod.time") func.gettime ;;
    "imod.rest") func.rest.request ;;
    *) exit 1 ;;
esac

exit 0

### vim:ts=4 sw=4 noet fdm=marker:
