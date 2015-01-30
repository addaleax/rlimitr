rlimitr
=======

get/set resource limits on command line in env(1)-style.

Usage
-----
Corresponds nearly 1:1 to that of env(1).

Usage examples:

* `rlimitr` lists the current resource limits
* `rlimitr RLIMIT_AS=s50M:h50M ls` sets the soft and hard address space limits to 50 MB and executes ls
* `rlimitr RLIMIT_AS=50M ls` sets only the soft limit and executes ls

See `rlimitr --help` for more details.

Installation
------------
You can compile via `make build` and, if you want to, install via `[sudo] make install`.
