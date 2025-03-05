#!/bin/bash
set -x
clear
make -B -j12 && ./cplq "$@"
