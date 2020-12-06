#!/bin/bash

# Script to test volume creation capability of gostcrypt

# Steps:
# - Call the createReference.sh script to create all types of volumes.
#    - if the command fails, program does not conform
# - Volumes are never constant and can not be compared to reference volumes
# - xmount.sh is ran over our new volumes

# getting folder under test
datafolder=$1

if [ "$datafolder" == "" ]; then
  echo "usage: ./create.sh <folder>"
  exit 1
fi

# creating new folder
mkdir "$datafolder/current"
cd "$datafolder" || exit 1

echo "Starting test"

# executing creation script
./createReference.sh "current/" > /dev/null

# Checking success
if [ $? -ne 0 ]; then
  echo "Creation failed"
  exit 1
else
  echo "Volumes created successfully"
fi

# going back to script dir
cd - || exit 1

# executing test mount script on created folder
./xmount.sh "$datafolder/current" > /dev/null

# Checking success
if [ $? -ne 0 ]; then
  echo "Mount tests unsuccessfuls"
  exit 2
else
  echo "Created volumes are valid."
fi

# cleaning dir
rm -rf "$datafolder/current"

# clean exit
exit 0
