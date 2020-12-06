#!/bin/bash

# Script to call test subscripts on default volumes
# this script must print UnitTest-like output (for easier formatting by other tools later)
# This way, results from ut can be merged with these..

# Expected trace (to remember output format needed)
#mainTests.sh:0:testit_mount_mount:PASS
#mainTests.sh:0:testit_mount_check:PASS
#mainTests.sh:0:testit_mount_list:PASS
#mainTests.sh:0:testit_mount_umount:PASS
#mainTests.sh:0:testit_create_create:PASS
#mainTests.sh:0:testit_create_check:PASS
#
#-----------------------
#6 Tests 0 Failures 0 Ignored
#OK

# checkerr <code> <expected> <name> <error msg>
checkerr() {
  TESTS=$(($TESTS + 1))
  if [ $1 == $2 ]; then
    FAILURES=$(($FAILURES + 1))
    echo "mainTests.sh:0:testit_$3:FAIL:$4"
  else
    echo "mainTests.sh:0:testit_$3:PASS"
  fi
}

cd src || exit

# calling mount script
./xmount.sh ../data > tmp.txt

ECODE=$?
ERR=$(tail -n 1 tmp.txt)

TESTS=0
FAILURES=0

checkerr $ECODE 1 "mount_mount" "$ERR"
checkerr $ECODE 2 "mount_check" "$ERR"
checkerr $ECODE 3 "mount_list" "$ERR"
checkerr $ECODE 4 "mount_umount" "$ERR"

# calling create script
./create.sh ../data > tmp.txt

ECODE=$?
ERR=$(tail -n 1 tmp.txt)

checkerr $ECODE 1 "create_create" "$ERR"
checkerr $ECODE 2 "create_check" "$ERR"

# end summary
echo ""
echo "-----------------------"
echo "$TESTS Tests $FAILURES Failures 0 Ignored"

# deleting logs
rm tmp.txt

# final echo and exit
if [ $FAILURES == 0 ]; then
  echo "OK"
  exit 0
else
  echo "FAILURE"
  exit 1
fi
