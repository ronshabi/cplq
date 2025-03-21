#!/bin/bash

# IMPORTANT: use `make test` to invoke this. Do NOT invoke directly.

# Script for testing, which includes:
# 1. Running the compiler on every test in tests/{,basic,given,switch}
#    All tests in these directories should pass.
# 2. Running the VM on the test output
# 3. Comparing VM output to the exp file (done through the VM itself)
#
# If one of these steps fail, the testsuite stops and reports an error.


for f in tests/{,basic/,given/,switch/}*.ou; do
    PUREFILE=${f%%.ou}
    echo "--" $f

    ./cplq "$PUREFILE.ou"
    if [[ "$?" != "0" ]]; then
        echo "FAILED COMPILING"
        exit 1
    fi

    unbuffer python3 vm/qvm.py "$PUREFILE.qud"
    if [[ "$?" != "0" ]]; then
        echo "FAILED EMULATING $PUREFILE.qud"
        exit 1
    fi

done

echo -e "\033[1;32m --- ALL TESTS PASSED --- \033[0m"