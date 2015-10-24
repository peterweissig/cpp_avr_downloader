#!/bin/sh

###############################################################################
#                                                                             #
# checkout.sh                                                                 #
# ===========                                                                 #
#                                                                             #
# Version: 1.0.0                                                              #
# Date   : 24.10.15                                                           #
# Author : Peter Weissig                                                      #
#                                                                             #
# For help or bug report please visit:                                        #
#   https://github.com/peterweissig/cpp_avr_downloader/                       #
###############################################################################

NAME_GIT_THIS="avr_downloader"
NAME_GIT_MAIN="main"
NAME_GIT_SUBS="basic comport file"

URL_GIT_BASE="https://github.com/peterweissig/"
URL_GIT_THIS="${URL_GIT_BASE}cpp_${NAME_GIT_THIS}.git"
URL_GIT_MAIN="${URL_GIT_BASE}cpp_${NAME_GIT_MAIN}.git"

PATH_SOURCE="src/"
NAME_CHECKOUT_SCRIPT="checkout.sh"
NAME_ARGUMENT_SCRIPT="dependency"

SRC_DIR="$(pwd)"

if [ "$#" -eq 0 ]; then
    echo "Running this script without parameters."
    echo "Therefore the project"
    echo "  \"${NAME_GIT_THIS}\""
    echo "will be checked out completely."
    echo ""

    echo ""
    echo "### checking out toplevel structure"
    if [ -d ".git" ]; then
        echo "This folder already is a git-repository!"
        return
    fi
    git init
    git pull "${URL_GIT_MAIN}"

    echo ""
    echo "### checking out main source code"
    mkdir -p "${PATH_SOURCE}${NAME_GIT_THIS}/"
    cd "${PATH_SOURCE}${NAME_GIT_THIS}/"

    if [ -d ".git" ]; then
        echo "This folder already is a git-repository!"
        return
    fi
    git init
    git pull "${URL_GIT_THIS}"

    echo ""
    echo "### checking out additional source code"
    if [ ! -f "${NAME_CHECKOUT_SCRIPT}" ]; then
        echo "Error - no checkout script"
        return
    fi
    . "${NAME_CHECKOUT_SCRIPT}" "${NAME_ARGUMENT_SCRIPT}"

    echo ""
    echo "### deleting this script"
    cd "${SRC_DIR}"
    rm "${NAME_CHECKOUT_SCRIPT}"

elif [ "$1" == "${NAME_ARGUMENT_SCRIPT}" ]; then
    echo "Running this script with argument \"$1\"."
    echo "Therefore all dependend sources of"
    echo "  \"${NAME_GIT_THIS}\""
    echo "will be checked out recursivly."
    echo ""

    for dependency in $NAME_GIT_SUBS ; do
        cd "${SRC_DIR}"

        if [ ! -d "../${dependency}" ]; then
            echo ""
            echo "### checking out ${dependency}"
            mkdir -p "../${dependency}"
            cd "../${dependency}"

            URL="${URL_GIT_BASE}cpp_${dependency}.git"
            git init
            git pull "${URL}"

            if [ -f "checkout.sh" ]; then
                sh "${NAME_CHECKOUT_SCRIPT}" "${NAME_ARGUMENT_SCRIPT}"
            fi
        fi
    done

else
    echo "Running this script with argument \"$1\"."
    echo "Error - expecting no argument or \"${NAME_ARGUMENT_SCRIPT}\"."
    return
fi
