#!/bin/bash

clear
make
if [[ "$?" != "0" ]]; then
    echo "ERROR: build failed"
    exit 1
fi


for f in tests/*.ou; do
    PUREFILE=${f%%.ou}
    echo "--" $PUREFILE

    if [[ "$PUREFILE" =~ "error" ]]; then
        echo "SKIP"
        continue
    fi

    ./cplq "$PUREFILE.ou"
    if [[ "$?" != "0" ]]; then
        echo "FAILED COMPILING"
        exit 1
    fi

    python3 vm/qvm.py "$PUREFILE.qud"
    if [[ "$?" != "0" ]]; then
        echo "FAILED EMULATING"
        exit 1
    fi

done