#!/bin/bash

# App name from args
if [ $# -eq 0 ]
then
    echo "Usage: sh update_app.sh <app_name>"
    exit
fi

# copy Makefile, platform and mission
cp ../../apps/$1/fsw/for_build/* ./$1/
cp ../../apps/$1/fsw/platform_inc/* ./inc/
cp ../../apps/$1/fsw/mission_inc/* ../mission_inc/

