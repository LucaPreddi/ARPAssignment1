#!/bin/bash

cd sources
cd executables
./master
mv debug.txt ..
cd ..
mv debug.txt logfile
