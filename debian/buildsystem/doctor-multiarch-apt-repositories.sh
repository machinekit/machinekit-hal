#!/usr/bin/env bash

#####################################################################
# Description:  doctor-multiarch-apt-repositories.sh
#
#               This file, 'doctor-multiarch-apt-repositories.sh', implements
#               simple function for solving issues originating from so-called
#               split repositories when one repository address is specific
#               to one or more architectures and similar problems
#
# Copyright (C) 2020    Jakub Fišer  <jakub DOT fiser AT eryaf DOT com>
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
#
######################################################################

################################################################################
# Global Variables Declarations
################################################################################

SOURCES_FILE=${SOURCES_FILE:-}
HOST_ARCHITECTURE=${HOST_ARCHITECTURE:-}
BUILD_ARCHITECTURE=${BUILD_ARCHITECTURE:-}

################################################################################
# Maintenance Functions
################################################################################

NAME=$(basename $0|sed 's/\(\..*\)$//')

function usage() {
    cat <<USAGEHEREDOC >&2
[$@]

Usage: $NAME [OPTIONS] ARGS...
Simple description on how to use this script...

OPTION      DESCRIPTION
==========  ==================================================================
-a          The HOST architecture (Debian nomenclature)
                -> For which architecture the binaries will be build
-b          The BUILD architecture (Debian nomenclature)
                -> On which architecture the binaries will be build
                   (Defaults to 'dpkg --print-architecture' output)
-s          Absolute path to sources.list file on which script will operate
                -> (Defaults to '/etc/apt/sources.list')
-h          Print this helpful text

USAGEHEREDOC
}

# TODO:
# This whole should be taken only as an initial commit
# The colorful output logging with icons and timestamps is currently missing 
# and the code should act accordingly when the output is being piped to other
# programs (the colors should be automatically turned off, icons and time
# stripped off)
_write_out() {
    local level=$1
    local message="${@:2}"
    
    printf "%b"          \
           "$message\n";
}

log_info(){
    _write_out "1" "$@"
} >&2
log_warning(){
    _write_out "2" "$@"
} >&2
log_error(){
    _write_out "3" "$@"
} >&2

success() {
    _write_out "5" "Script $NAME was successful!"
    exit 0
} >&2
failure() {
    _write_out "6" "Script $NAME failed!"
    exit 1
} >&2

_process_options(){
    while getopts ":a:b:s:h" opt; do
    	case $opt in
    	    a) HOST_ARCHITECTURE="${OPTARG}" ;;
            b) BUILD_ARCHITECTURE="${OPTARG}" ;;
            s) SOURCES_FILE="${OPTARG}" ;;
    	    h) usage "Print help" ; exit 0;;
    	    :) usage "Option -${OPTARG} requires an argument." ; exit 1 ;;
    	    \?) usage "Invalid option -${OPTARG}" ; exit 1;;
    	esac
    done
    shift $((OPTIND-1))

    if [[ "$HOST_ARCHITECTURE" == "" ]]
    then
        usage "Missing required -a parameter"
        failure
    fi
    if [[ "$BUILD_ARCHITECTURE" == "" ]]
    then
        BUILD_ARCHITECTURE="$(dpkg --print-architecture)"
        if (( $? != 0 ))
        then
            log_error "Command 'dpkg --print-architecture' failed!"
            failure
        fi
    fi
    if [[ "$SOURCES_FILE" == "" ]]
    then
        SOURCES_FILE="/etc/apt/sources.list"
    fi
}

################################################################################
# Program Functions
################################################################################

lsb_release_installed(){
    dpkg-query -W 'lsb-release' > /dev/null 2>&1
    local retval="$?"

    if (( retval != 0 ))
    then
        log_warning "The lsb_release package has to be installed!"
        log_info ""
        return -1
    fi
    return 0
}

am_i_root(){
    local USER_ID="$(id -u)"

    if (( USER_ID != 0 ))
    then
        log_error "Script $NAME is not running with root priviledges."
        return -1
    fi
    return 0
}

add_architecture_to_existing_entry(){
    local REGEX="^(deb(-src){0,1}[[:space:]]\[arch=.{1,})\][[:space:]].{0,}$3.{0,}$"
    local OUTPUT=""
    local LINE=""

    if (( $# != 3 ))
    then
        log_warning "Arguments passed to add_architecture_to_existing_entry()" \
                    "function must be\n"                                       \
                    "\$1 FILE_TO_OPERATE_ON"                                   \
                    "\$2 ARCHITECTURE_TO_ADD"                                  \
                    "\$3 REGEX_ENTRY_FILTER"
        return -1
    fi

    while IFS= read -r LINE
    do
        if [[ $LINE =~ $REGEX ]]
        then
            OUTPUT+="${LINE:0:${#BASH_REMATCH[1]}},$2${LINE:${#BASH_REMATCH[1]}}\n"
            continue
        fi
        OUTPUT+="$LINE\n"
    done < ${1}

    echo -e "$OUTPUT" >| ${1}
    return 0  
}

copy_and_transform_repositories(){
    local REGEX="^deb(-src){0,1}[[:space:]]\[arch=(.{1,})\][[:space:]].{0,}$5.{0,}$"
    local OUTPUT=""
    local LINE=""

    if (( $# != 5 ))
    then
        log_warning "Arguments passed to copy_and_transform_repositories()" \
                    " function must be"                                     \
                    "\$1 FILE_TO_OPERATE_ON"                                \
                    "\$2 REPOSITORY_BASE_ADDRESS"                           \
                    "\$3 NEW_REPOSITORY_BASE_ADDRESS"                       \
                    "\$4 NEW_REPOSITORY_ARCHITECTURE"                       \
                    "\$5 REGEX_ENTRY_FILTER"
        return -1
    fi

    if [[ "$2" == "$3" ]]
    then
        log_error "Apt does not allow multiple lines with the same repository" \
                  "address, which will be the output of this action!"
        return -1
    fi

    while IFS= read -r LINE
    do
        if [[ $LINE =~ $REGEX ]]
        then
            local SUBSTITUTION="${LINE/${BASH_REMATCH[2]}/$4}"
            SUBSTITUTION="${SUBSTITUTION/$2/$3}"
            OUTPUT+="$LINE\n$SUBSTITUTION\n"
            continue
        fi
        OUTPUT+="$LINE\n"
    done < ${1}

    echo -e "$OUTPUT" >| ${1}
    return 0
}

lock_to_architecture(){
    local REGEX="^(deb(-src){0,1}[[:space:]]).{0,}$3.{0,}$"
    local OUTPUT=""
    local LINE=""

    if (( $# != 3 ))
    then
        log_warning "Arguments passed to lock_to_architecture()" \
                    "function must be\n"                         \
                    "\$1 FILE_TO_OPERATE_ON\n"                   \
                    "\$2 ARCHITECTURE\n"                         \
                    "\$3 REGEX_ENTRY_FILTER"
        return -1
    fi

    while IFS= read -r LINE
    do
        if [[ $LINE =~ $REGEX ]]
        then
            local POS="${#BASH_REMATCH[1]}"
            OUTPUT+="${LINE:0:POS}[arch=$2] ${LINE:POS}\n"
            continue
        fi
        OUTPUT+="$LINE\n"
    done < ${1}

    echo -e "$OUTPUT" >| ${1}
    return 0
}

attend_to_ubuntu(){
    local retval=-1
    lock_to_architecture "$SOURCES_FILE" "$BUILD_ARCHITECTURE" "ubuntu\.com"
    retval=$?
    if (( retval != 0 ))
    then
        retun ${retval}
    fi
    if [[ "$BUILD_ARCHITECTURE" == "$HOST_ARCHITECTURE" ]]
    then
        log_info "Ubuntu doesn't need any additional changes for native build," \
                 "all is well in its $SOURCES_FILE.\n"
    else
        local address_from=""
        case "$BUILD_ARCHITECTURE" in
            amd64 | i386)
                local address_from="ubuntu.com/ubuntu"
                local address_from_regex="ubuntu\.com\/ubuntu"
                case "$HOST_ARCHITECTURE" in
                    amd64 | i386)
                        add_architecture_to_existing_entry "$SOURCES_FILE"       \
                                                           "$HOST_ARCHITECTURE"  \
                                                           "$address_from_regex"
                        if (( retval != 0 ))
                        then
                            return ${retval}
                        fi
                    ;;
                    *)
                        local ports_base="ports.$address_from-ports"
                        local security_base="security.$address_from"
                        local security_regex="$address_from_regex\/[[:space:]][^ -]{1,}-security"
                        copy_and_transform_repositories "$SOURCES_FILE"      \
                                                        "$security_base"     \
                                                        "$ports_base"        \
                                                        "$HOST_ARCHITECTURE" \
                                                        "$security_regex"
                        if (( retval != 0 ))
                        then
                            return ${retval}
                        fi
                        local archive_base="archive.$address_from"
                        local archive_regex="$address_from_regex\/[[:space:]][^ -]{1,}((-backports)|(-updates)|([[:space:]]))"
                        copy_and_transform_repositories "$SOURCES_FILE"      \
                                                        "$archive_base"      \
                                                        "$ports_base"        \
                                                        "$HOST_ARCHITECTURE" \
                                                        "$archive_regex"
                        if (( retval != 0 ))
                        then
                            return ${retval}
                        fi
                    ;;
                esac
            ;;
            *)
                local address_from="ports.ubuntu.com/ubuntu-ports"
                local address_from_regex="ports\.ubuntu\.com\/ubuntu-ports"
                case "$HOST_ARCHITECTURE" in
                    amd64 | i386)
                        local security_base="security.ubuntu.com/ubuntu"
                        local security_regex="$address_from_regex\/[[:space:]][^ -]{1,}-security"
                        copy_and_transform_repositories "$SOURCES_FILE"      \
                                                        "$address_from"      \
                                                        "$security_base"     \
                                                        "$HOST_ARCHITECTURE" \
                                                        "$security_regex"
                        if (( retval != 0 ))
                        then
                            return ${retval}
                        fi
                        local archive_base="archive.ubuntu.com/ubuntu"
                        local archive_regex="$address_from_regex\/[[:space:]][^ -]{1,}((-backports)|(-updates)|([[:space:]]))"
                        copy_and_transform_repositories "$SOURCES_FILE"      \
                                                        "$address_from"      \
                                                        "$archive_base"      \
                                                        "$HOST_ARCHITECTURE" \
                                                        "$archive_regex"
                        if (( retval != 0 ))
                        then
                            return ${retval}
                        fi
                    ;;
                    *)
                        add_architecture_to_existing_entry "$SOURCES_FILE"       \
                                                           "$HOST_ARCHITECTURE"  \
                                                           "$address_from_regex" 
                        if (( retval != 0 ))
                        then
                            return ${retval}
                        fi                    
                    ;;
                esac
            ;;
        esac
        log_info "Remote repositories for architecture $HOST_ARCHITECTURE" \
                 "for multiarch build stored in $SOURCES_FILE.\n"
    fi
    return 0
}

attend_to_debian(){
    local retval=-1
    lock_to_architecture "$SOURCES_FILE" "$BUILD_ARCHITECTURE" "debian\.org"
    retval=$?
    if (( retval != 0 ))
    then
        retun ${retval}
    fi
    if [[ "$BUILD_ARCHITECTURE" == "$HOST_ARCHITECTURE" ]]
    then
        log_info "Debian doesn't need any additional changes for native build" \
                 "in its $SOURCES_FILE.\n"
    else
        add_architecture_to_existing_entry "$SOURCES_FILE"      \
                                           "$HOST_ARCHITECTURE" \
                                           "debian\.org\/debian"
        if (( retval != 0 ))
        then
            retun ${retval}
        fi
        log_info "Remote repositories for architecture $HOST_ARCHITECTURE" \
                 "added for multiarch build stored in $SOURCES_FILE.\n"
    fi
    return 0
}

################################################################################
# Main Function
################################################################################

_main(){
    local retval=-1

    am_i_root
    retval=$?
    if (( retval != 0 ))
    then
        failure
    fi
    
    lsb_release_installed
    retval=$?
    if (( retval != 0 ))
    then
        failure
    fi

    RUNNING_DISTRIBUTION="$(lsb_release -is | tr '[:upper:]' '[:lower:]')"

    case $RUNNING_DISTRIBUTION in
        debian)
            attend_to_debian
            retval=$?
            if (( retval != 0 ))
            then
                failure
            fi
        ;;
        ubuntu)
            attend_to_ubuntu
            retval=$?
            if (( retval != 0 ))
            then
                failure
            fi
        ;;
    esac

    local OUTPUT="$(<${SOURCES_FILE})"
    log_info "Final state of $SOURCES_FILE:\n"                                 \
             "=============================================================\n" \
             "$OUTPUT\n"                                                       \
             "=============================================================\n"
    
    success
}

################################################################################
# Start of execution
################################################################################

_process_options "$@"
_main
# Chatch all unaddressed exit situations
log_error "There was an error in script execution!"
failure
