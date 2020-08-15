#!/bin/bash

#####################################################################
# Description:  base-entrypoint.sh
#
#               This file, 'base-entrypoint.sh', implements Docker entrypoint
#               script used for Machinekit-HAL varieté of images
#
# Copyright (C) 2020            Jakub Fišer  <jakub DOT fiser AT eryaf DOT com>
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

PASSED_USER_UID=$(id -u)
PASSED_USER_GID=$(id -g)
IMAGE_USER_UID=$(id -u ${USER})
IMAGE_USER_GID=$(id -g ${USER})

# Only allow running when user is specified and do not allow root user period
if (( PASSED_USER_UID == 0 || PASSED_USER_GID == 0 ))
then
    printf "%b"                                                                    \
           "Machinekit-HAL entrypoint: Running as the root is not allowed.\n"      \
           "Passed user ID:${PASSED_USER_UID} and group ID:${PASSED_USER_GID}.\n";
    exit 1
fi
# Catch cases when user pass -e USER to Docker run command
if ! [[ $PASSED_USER_UID =~ ^[0-9]+$ ]]
then
    printf "%b"                                                                 \
           "Machinekit-HAL entrypoint: Don not pass user ${USER} with -e.\n" \
           "EXITING.\n";                                                        \
    exit 1
fi

if (( PASSED_USER_UID == IMAGE_USER_UID &&
      PASSED_USER_GID == IMAGE_USER_GID ))
then
    printf "%b"                                                                \
           "Machinekit-HAL entrypoint: Nothing needs to be done.\n"            \
           "Passed user ID:${PASSED_USER_UID} and group ID:${PASSED_USER_GID}" \
           " is the same as UID and GID of IMAGE user ${USER}.\n"
else
    printf "%b"                                                                 \
           "Machinekit-HAL entrypoint: Changing burnt-in user ${USER} UID and/" \
           "or GID to ones passed by --user run option.\n"                      \
           "All files under / belonging to old user ID:${IMAGE_USER_UID} or"    \
           " group ID:${IMAGE_USER_GID} will be updated to new values of user"  \
           " ID:${PASSED_USER_UID} and group ID:${PASSED_USER_GID}.\n"

    (
        machinekit-fixuid
    )
fi

# Set up new HOME environment variable if nothing was passed to container
if [[ -z "${HOME}" || "${HOME}" == "/" ]]
then
    ENT="$(getent passwd "${USER}")"
    NEW_HOME=$(echo $ENT | cut -d : -f 6)
    export HOME=${NEW_HOME}
fi

# Prepare environment for process
ENVIRONMENT_DIRECTORY="/opt/environment"
mapfile -d '' -t ARRAY_ENV < <(find ${ENVIRONMENT_DIRECTORY} -type f -print0 \
    | xargs -0 -n1 -I '{}' realpath -e -z '{}')

for FILE in "${ARRAY_ENV[@]}"
do
    source ${FILE}
done

printf "ENTRYPOINT END\n======================================================\n"
exec "$@"
