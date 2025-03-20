#!/bin/bash

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