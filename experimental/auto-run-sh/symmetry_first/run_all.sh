#!/bin/bash

set -e

SCRIPT_DIR=$(dirname "$0")


for script in "$SCRIPT_DIR"/*.sh; do
    if [[ "$script" == "$SCRIPT_DIR/run_all.sh" ]]; then
        continue
    fi


    if [[ -f "$script" ]]; then
        echo "----------------------------------------------"
        echo "SCRIPT RUNNING: $(basename "$script")"
        echo "----------------------------------------------"
        chmod +x "$script"   
        bash "$script"  
        echo "COMPLETE: $(basename "$script")"
        echo
    fi
done