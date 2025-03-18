#!/bin/bash

clear
make
if [[ "$?" != "0" ]]; then
    echo "ERROR: build failed"
    exit 1
fi


for f in tests/*.ou; do
    PUREFILE=${f%%.ou}
    echo "--" $f

    if [[ "$PUREFILE" =~ "error" ]]; then
        echo "SKIP"
        continue
    fi

    ./cplq "$PUREFILE.ou"
    if [[ "$?" != "0" ]]; then
        echo "FAILED COMPILING"
        exit 1
    fi

    unbuffer python3 vm/qvm.py "$PUREFILE.qud" # | tee "$PUREFILE.vm"
    if [[ "$?" != "0" ]]; then
        echo "FAILED EMULATING $PUREFILE.qud"
        exit 1
    fi

    # if [[ -f "$PUREFILE.vm" ]]; then
    #     diff -t "$PUREFILE.exp" "$PUREFILE.vm"

    # #     if [[ "$?" != "0" ]]; then
    # #         echo "Failed: $PUREFILE.qud: different outputs"
    # #         exit 1
    # #     fi
    # # fi

done