#!/bin/bash

if [ $# -eq 0 ]
then
 echo "Usage: ./install.sh <pathname> ";
 exit;
fi

if [ ! -d $1 ]
then
 echo "Error: Directory $1 DOES NOT exist.";
 while true; do
  read -p "Do you wish to create $1 directory? [Y/n] " yn
  case $yn in
   [Y]* ) mkdir $1; break;;
   [n]* ) exit;;
   * ) "Please, answer Y for yes or n for no.";;
  esac
 done
fi


echo "Begin program installation on $1 ... ";

mv run.sh $1
mv help.sh $1
unzip sources.zip -d $1;

echo "Begin sources' compilation ...";

cd $1
export DIR=$1
cd sources
cd dcommand
gcc -o command command.c
mv command ..
cd ..
cd dinspection
gcc -o inspection inspection.c
mv inspection ..
cd ..
cd dmotor_x
gcc -o motor_x motor_x.c
mv motor_x ..
cd ..
cd dmotor_z
gcc -o motor_z motor_z.c
mv motor_z ..
cd ..
cd dmaster
gcc -o master master.c
mv master ..
cd ..
cd dwatchdog
gcc -o watchdog watchdog.c
mv watchdog ..
cd ..
mkdir executables
mv command executables
mv inspection executables
mv motor_x executables
mv motor_z executables
mv master executables
mv watchdog executables

echo "You can run the program in $1 with ./run.sh ";

