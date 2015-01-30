#!/bin/sh
set -e
set -v
./rlimitr >/dev/null
./rlimitr RLIMIT_AS=50M ./rlimitr | grep RLIMIT_AS | grep -q 0x3200000
env RLIMIT_AS=50M ./rlimitr ./rlimitr | grep RLIMIT_AS | grep -q 0x3200000
