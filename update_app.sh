#!/bin/bash

# App name from args
if [ $# -eq 0 ]
then
    echo "Usage: sh update_app.sh <app_name>"
    exit
fi

# Some benchmarks have multiple apps
for D in ./bench/$1/apps/*; do
    if [ -d "${D}" ]; then
        echo "${D}"   # your processing here
        cp -r ${D} ./cFE/apps/`basename ${D}`
    fi
done

cp ./bench/$1/cfe_es_startup.scr ./cFE/build/cpu1/exe/cfe_es_startup.scr
cp ./bench/$1/Makefile ./cFE/build/cpu1/Makefile
