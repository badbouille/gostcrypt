#!/bin/bash

# Script to test volume mount capability of gostcrypt

# Steps:
# - Call the createReference.sh script to create all types of volumes.
#    - if the command fails, program does not conform
# - Volumes are never constant and can not be compared to reference volumes
# - xmount.sh is ran over our new volumes

# update path if needed
GC=../../../build/linux/release/bin/gc_cmdline

# Passwords
PASS_A="ASSURANCETOURIXX"

# getting folder under test
datafolder=$1

if [ "$datafolder" == "" ]; then
  echo "usage: ./xmount.sh <folder>"
  exit 1
fi

# Cheking that nothing is mounted yet
[ ! "$($GC list)" == "" ] && exit 1

# Function to check the content of the mounted volume
checkcontent() {
  folder=$1

  # Cheking that folder is not empty
  [ "$folder" == "" ] && exit 2

  # testing secret A
  content=$(cat "$folder/secretA.txt")
  if [ ! "$content" == "Abraracourcix" ]; then
    echo "Secret A not found"
    exit 2
  fi

  # testing secret B
  content=$(cat "$folder/folder/secretB.txt")
  if [ ! "$content" == "Bonemine" ]; then
    echo "Secret B not found"
    exit 2
  fi

  # testing secret C
  content=$(cat "$folder/folder/secretC.txt")
  if [ ! "$content" == "Cétautomatix" ]; then
    echo "Secret C not found"
    exit 2
  fi

  # tests ok
}

# creating new folder
mkdir "$datafolder/mountpoints"

echo "Starting test"

# for all volumes of this folder
for volume in $(find "$datafolder" -maxdepth 1 -name "v_*"); do

  # gettting volume name
  volumename=$(basename "$volume")
  mountpath="$datafolder/mountpoints/${volumename}_mount"

  echo "Mounting $volumename"

  # getting filesystem
  FS=$(echo "$volumename" | cut -d_ -f4)

  # getting password presence
  P=$(echo "$volumename" | cut -d_ -f5 | grep p)

  # if password is present
  if [ ! "$P" == "" ]; then
    # set password option to PASS_A
    P="-p$PASS_A"
  fi

  # creating mount folder
  mkdir "$mountpath" || exit 1

  # mounting volume
  $GC mount "$volume" "$mountpath" -f"$FS" $P

  # Checking success
  if [ $? -ne 0 ]; then
    echo "Could not mount"
    exit 1
  fi

  # checking content (only for ext2 volumes)
  if [ "$FS" == "ext2" ]; then
    echo "Checking content of ${volumename}_mount"
    checkcontent "$datafolder/mountpoints/${volumename}_mount"
  fi

  # volume has been mounted and content checked. everything worked.
done

# checking mounted volumes
Nmounted=$($GC list | wc -l)
Nvolumes=$(find "$datafolder" -maxdepth 1 -name "v_*" | wc -l)

if [ ! "$Nmounted" == "$Nvolumes" ]; then
  echo "All volumes not mounted"
  exit 3
else
  echo "All volumes mounted at once successfully"
fi

# Unmounting all volumes at once (except last three)
for volume in $(find "$datafolder" -maxdepth 1 -name "v_*" | head -n -3); do

  # gettting volume name
  volumename=$(basename "$volume")
  mountpath="$datafolder/mountpoints/${volumename}_mount"

  # unmounting volume
  $GC umount "$mountpath"

  # Checking success
  if [ $? -ne 0 ]; then
    echo "Could not unmount $mountpath"
    exit 4
  fi

done

# checking that everything is unmounted except three
Nmounted=$($GC list | wc -l)

if [ ! "$Nmounted" == "3" ]; then
  echo "Could not unmount everything"
  exit 4
else
  echo "All volumes but 3 unmounted successfully"
fi

# unmounting all remaining volumes
$GC umount -a

# Checking success
if [ $? -ne 0 ]; then
  echo "Could not unmount-all"
  exit 5
fi

# checking that everything is unmounted
Nmounted=$($GC list | wc -l)

if [ ! "$Nmounted" == "0" ]; then
  echo "Could not unmount-all everything"
  exit 5
else
  echo "All volumes unmounted successfully"
fi

# cleaning dir
rm -rf "$datafolder/mountpoints"

# clean exit
exit 0
