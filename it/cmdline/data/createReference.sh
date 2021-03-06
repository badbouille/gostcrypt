#!/bin/bash

# Script to generate the volumes in the directory

# Note:
# Normally, the volumes should be created by-hand, to ensure the program is conform to the standard
# In reality, it is impossible to create by hand a perfect volume.
# The unit tests already are there to make sure the underlying functions (Volumes, Algorithms, ..) works as specified.

# The volumes are generated from a stable version of the program and supposed to be analysed by hand (make sure the
# actual volumes make sense. These volumes are added in git as a regression test, to make sure the stable old volumes
# still works as intended. The script is only intended for easier update of theses volumes

# update path if needed
GC=../../../build/linux/release/bin/gc_cmdline
MOUNTPOINT=mountpoint
DIR=$1

# Passwords
PASS_A="ASSURANCETOURIXX"

createfolder() {
  cd $MOUNTPOINT || quit 1
  echo "Abraracourcix" > secretA.txt
  mkdir folder
  echo "Bonemine" > folder/secretB.txt
  echo "Cétautomatix" > folder/secretC.txt
  cd - || quit 1
  # add some other content here
}

setup() {
  # checking if other mounted
  if [ "$($GC list)" ]; then
      echo "Already mounted volumes" && exit 1;
  fi
  
  # creating mountpoint directory
  mkdir $MOUNTPOINT
}

quit() {
  # removing mountpoint directory
  rmdir $MOUNTPOINT
  exit "$1"
}

cleanup() {
  if [ $? -ne 0 ]; then
    echo "Creation error"
    quit 1
  fi
  $GC umount "$MOUNTPOINT"
  if [ $? -ne 0 ]; then
    echo "Unmount error"
    quit 1
  fi
}

# setup env
setup

# VOLUMES

# ECB XOR NONE

$GC create "${DIR}v_std_ecbxor_none" "$MOUNTPOINT" -fnone -s1K
cleanup

$GC create "${DIR}v_std_ecbxor_none_b1k" "$MOUNTPOINT" -fnone -s1K -b1K
cleanup

$GC create "${DIR}v_std_ecbxor_none_b16" "$MOUNTPOINT" -fnone -s1K -b16
cleanup

$GC create "${DIR}v_std_ecbxor_none_p" "$MOUNTPOINT" -fnone -s1K -p$PASS_A
cleanup

$GC create "${DIR}v_std_ecbxor_none_sbp" "$MOUNTPOINT" -fnone -s2K -p$PASS_A -b512
cleanup

# ECB XOR EXT2

$GC create "${DIR}v_std_ecbxor_ext2" "$MOUNTPOINT" -s2M
createfolder
cleanup

# note: around 1.2M is the minimum size for use with lwext4. mkfs.ext4 can create smaller filesystems.
$GC create "${DIR}v_std_ecbxor_ext2_b10k" "$MOUNTPOINT" -s1200K -b10K
createfolder
cleanup

$GC create "${DIR}v_std_ecbxor_ext2_sbp" "$MOUNTPOINT" -s2M -p$PASS_A -b16
createfolder
cleanup

# Exiting program
quit 0
