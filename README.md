
multirun
========

A very simple Unix utility to run multiple commands concurrently. It is mostly useful as a lightweight solution to run
multiple servers in docker containers.

Usage: multirun "command1" "command2" ...

A few comments about its behavior:

* If any of its children exits it will send SIGTERM to all the others.
* It will ignore SIGINT and SIGTERM but will forward them to all its children.
* It will only terminate when all its children have exited.
* If any of its children has exited in an abnormal way it will return -1.

Installation
------------

Currently the only way is to compile it. But since it has no dependencies it's really easy to do. You just need git,
make and cc.

    git clone --branch 0.1.0 https://github.com/nicolas-van/multirun.git
    cd multirun
    make
    cp multirun /bin
    cd ..
    rm -rf multirun
    
