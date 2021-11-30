#!/bin/bash

# Running the program and moving the debug.txt from
# the executables folder to a new one dedicated to 
# the logfile.

cd $X
cd executables
./master
mv debug.txt ..
cd ..
mv debug.txt logfile
