#!/bin/bash

# Check for log file argument
if [ $# -ne 1 ]; then
    echo "Usage: $0 <logfile>"
    exit 1
fi

logfile="$1"

# Associative arrays to track commands
declare -A running
declare -A found
declare -A finished

# Read the file line by line
while IFS= read -r line; do
    # Match "Running: <command>" (allow leading spaces)
    if [[ $line =~ Running:[[:space:]]+(.+)$ ]]; then
        cmd="${BASH_REMATCH[1]}"
        running["$cmd"]=1
    fi

    # Match "Found:<command>" (allow optional space, require at least one character after colon)
    if [[ $line =~ Found:[[:space:]]*(.+)$ ]]; then
        cmd="${BASH_REMATCH[1]}"
        found["$cmd"]=1
    fi

    # Match "Finished: <command>" (allow leading spaces)
    if [[ $line =~ Finished:[[:space:]]+([^,]+)(,[[:space:]]*it took:.*)?$ ]]; then
        cmd="${BASH_REMATCH[1]}"
        finished["$cmd"]=1
    fi
done < "$logfile"

# Report commands that are missing either a "Found:" or a "Finished:" line
for cmd in "${!running[@]}"; do
    if [[ -z ${found[$cmd]} ]] && [[ -z ${finished[$cmd]} ]]; then
        echo "$cmd"
    fi
done