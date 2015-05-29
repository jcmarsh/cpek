# cpek
CFE Performance Evaluation Kit

Intended to evaluate NASA's cFE on a Gomspace Nanomind.

## Benchmarks

Right now only a few custom benchmarks:
* app_ping - How many round trip messages (right now just empty) can be sent between two apps in a second.
* hw_ping - How many round trip messages between an app and a piece of hardware be send in a second. Reads a parameter from a Sinclair Planetary reaction wheel over I2C. Depends on I2C initialization speed. Should add different hardware / interfaces.
* flat_sat - Not working. Intended to simulate real flight software with multiple pieces of hardware and computation intensive software (ACS).

## Utilities
* `./new_app/` - A script (`new_application.py`) that creates a slew of files based off of template files and using the names provided in `var.table`. Based off of the cFE sample_app.
* `update_app.sh - Script to copy files into the build directory for an application. Should really be part of Make system if I could figure it out.
