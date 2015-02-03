#!/bin/sh
set -e
set -v
./rlimitr >/dev/null
env RLIMIT_AS=50M ./rlimitr ./rlimitr | grep RLIMIT_AS | grep -Fq 0x3200000
./rlimitr RLIMIT_AS=50M ./rlimitr | grep RLIMIT_AS | grep -Fq 0x3200000
./rlimitr RLIMIT_AS=s50M:h50M ./rlimitr | grep RLIMIT_AS | grep -Fq s0x3200000:h0x3200000
./rlimitr RLIMIT_AS=RLIM_INFINITY ./rlimitr | grep RLIMIT_AS | grep -Fq RLIMIT_AS=sRLIM_INFINITY
./rlimitr RLIMIT_NOFILE=+1 ./rlimitr | grep RLIMIT_NOFILE | grep -Fq 0x1
./rlimitr RLIMIT_NOFILE=-1 ./rlimitr | grep RLIMIT_NOFILE | grep -Fq ff
./rlimitr RLIMIT_AS=1048576K ./rlimitr | grep RLIMIT_AS | grep -Fq s0x40000000
./rlimitr RLIMIT_AS=1G ./rlimitr | grep RLIMIT_AS | grep -Fq s0x40000000
./rlimitr RLIMIT_AS=1T ./rlimitr | grep RLIMIT_AS | grep -Fq s0x10000000000
./rlimitr RLIMIT_AS=1P ./rlimitr | grep RLIMIT_AS | grep -Fq s0x4000000000000
./rlimitr RLIMIT_NICE=garbage ./rlimitr 2>&1 | grep -Fqi warning
./rlimitr RLIMIT_NICE=s10:garbage ./rlimitr 2>&1 | grep -Fqi warning
./rlimitr RLIMIT_AS=-1 ./rlimitr 2>&1 | grep -Fqi warning
./rlimitr RLIMIT_AS=s50M:h25M ./rlimitr 2>&1 | grep -Fqi 'Invalid argument'
./rlimitr RLIMIT_NOSUCHRES=s50M:h25M ./rlimitr 2>&1 | grep -Fqi error
./rlimitr --help | grep -Fqi -- --version
./rlimitr --version | grep -Fqi -- GPL
./rlimitr -- ls | grep -Fqi -- rlimitr
./rlimitr ls | grep -Fqi -- rlimitr
