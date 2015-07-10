# cpek
CFE Performance Evaluation Kit

Intended to evaluate NASA's cFE on a Gomspace Nanomind.

## Benchmarks

Right now only a few custom benchmarks:
* app_ping - How many round trip messages (right now just empty) can be sent between two apps in a second.
* benchmarks - Common benchmark utilities such are Whetstone and Dhrystone
* hw_ping - How many round trip messages between an app and a piece of hardware be send in a second. Reads a parameter from a Sinclair Planetary reaction wheel over I2C. Depends on I2C initialization speed. Should add different hardware / interfaces.
* flat_sat - Not working. Intended to simulate real flight software with multiple pieces of hardware and computation intensive software (ACS).

## Utilities
* `./new_app/` - A script (`new_application.py`) that creates a slew of files based off of template files and using the names provided in `var.table`. Based off of the cFE sample_app.
* `update_app.sh - Script to copy files into the build directory for an application (for CFS). Should really be part of Make system if I could figure it out.

## Installation

cFE - To run a benchmark in cFE:
* Copy apps from `cpek/cFE/<benchmark>/apps/` into app folder of a fresh cFE install.
* Update the apps:
  * For each app, run `sh update_app.sh <app_name> from the platform build directory (`build/<platform>/`)
* Copy corresponding Makefile from `cpek/cFE/<benchmarks>/` into `build/<platform>/`
* Copy `cpek/cFE/<benchmark>/startuptable.c` to `build/<platform>/startuptab/` (nanomind platform only).
* Copy `cpek/cFE/<benchmark>/cfe_es_startup.scr` to build/<platform>/exe/`
* From top level of cFE source, run `. ./setvars.sh`
* From `/build/<platform>/` run `make`
* From `/build/<platform>/nanomind` run `make program` (nanomind only)
* Make sure to update the run script on the nanomind as well (using `csp-term`)

FreeRTOS - To run a benchmark in FreeRTOS:
* Copy files from `cpek/FreeRTOS/<benchmark>/include` and `cpek/FreeRTOS/<benchmark>/src` into the corresponding folders of a fresh FreeRTOS source (from GomSpace) directory.
* From top level for FreeRTOS source:
  * ./configure-a712-nostorage.sh 
  * ./waf build
  * ./waf program

Dhrystone - Original is from: http://zenit.senecac.on.ca/wiki/index.php/Dhrystone_howto

Modified to run on the NanoMind in a cFE app / FreeRTOS task.
  `wget http://www.sfr-fresh.com/unix/privat/old/dhrystone-2.1.tar.gz`
  `tar xf dhrystone-2.1.tar.gz`

Update the Makefile, make, and run (enter at least 100000000)

Divide result by 1757 to convert to DMIPS.
