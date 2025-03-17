#!/bin/bash
set -x
clear
make && ./cplq tests/$1.ou && python3 vm/qvm.py tests/$1.qud
