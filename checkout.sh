#!/bin/sh

###############################################################################
#                                                                             #
# checkout.sh                                                                 #
# ===========                                                                 #
#                                                                             #
###############################################################################
#                                                                             #
# github repository                                                           #
#   https://github.com/peterweissig/cpp_avr_downloader                        #
#                                                                             #
###############################################################################
#                                                                             #
# New BSD License                                                             #
#                                                                             #
# Copyright (c) 2015-2016, Peter Weissig                                      #
# All rights reserved.                                                        #
#                                                                             #
# Redistribution and use in source and binary forms, with or without          #
# modification, are permitted provided that the following conditions are met: #
#     * Redistributions of source code must retain the above copyright        #
#       notice, this list of conditions and the following disclaimer.         #
#     * Redistributions in binary form must reproduce the above copyright     #
#       notice, this list of conditions and the following disclaimer in the   #
#       documentation and/or other materials provided with the distribution.  #
#     * The names of its contributors may not be used to endorse or promote   #
#       products derived from this software without specific prior written    #
#       permission.                                                           #
#                                                                             #
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" #
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE   #
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE  #
# ARE DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY      #
# DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES  #
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR          #
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER  #
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT          #
# LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY   #
# OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH #
# DAMAGE.                                                                     #
#                                                                             #
###############################################################################

NAME_GIT_THIS="avr_downloader"
NAME_GIT_SUBS="basic comport file"

###############################################################################
NAME_GIT_MAIN="main"
URL_GIT_BASE="https://github.com/peterweissig/"
URL_GIT_THIS="${URL_GIT_BASE}cpp_${NAME_GIT_THIS}.git"
URL_GIT_MAIN="${URL_GIT_BASE}cpp_${NAME_GIT_MAIN}.git"

PATH_SOURCE="src/"
NAME_CHECKOUT_SCRIPT="checkout.sh"
NAME_ARGUMENT_SCRIPT="dependency"

###############################################################################
if [ "$#" -eq 0 ]; then
    echo "Running this script without parameters."
    echo "Therefore the project"
    echo "  \"${NAME_GIT_THIS}\""
    echo "will be checked out completely."
    echo ""

    SRC_DIR_MAIN="$(pwd)"

    echo ""
    echo "### checking out toplevel structure"
    if [ -d ".git" ]; then
        echo "This folder already is a git-repository!"
        return
    fi
    git init
    git pull "${URL_GIT_MAIN}"
    if [ $? -ne 0 ]; then return; fi

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
    if [ $? -ne 0 ]; then return; fi

    echo ""
    echo "### checking out additional source code"
    if [ ! -f "${NAME_CHECKOUT_SCRIPT}" ]; then
        echo "Error - no checkout script"
        return
    fi
    sh "${NAME_CHECKOUT_SCRIPT}" "${NAME_ARGUMENT_SCRIPT}"
    if [ $? -ne 0 ]; then return; fi

    echo ""
    echo "### deleting this script"
    cd "${SRC_DIR_MAIN}"
    rm "${NAME_CHECKOUT_SCRIPT}"

###############################################################################
elif [ "$1" = "${NAME_ARGUMENT_SCRIPT}" ]; then
    echo "Running this script with argument \"$1\"."
    echo "Therefore all dependend sources of"
    echo "  \"${NAME_GIT_THIS}\""
    echo "will be checked out recursivly."
    echo ""

    SRC_DIR_DEPENDENCY="$(pwd)"

    for dependency in $NAME_GIT_SUBS ; do
        cd "${SRC_DIR_DEPENDENCY}"

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

###############################################################################
else
    echo "Running this script with argument \"$1\"."
    echo "Error - expecting no argument or \"${NAME_ARGUMENT_SCRIPT}\"."
    return
fi
