
multirun
========

A very simple Unix utility to run multiple commands concurrently. It is mostly aimed as a much lighter alternative to
supervisord to run multiple servers in the same docker container.

Usage: multirun "command1" "command2" ...

A few comments about its behavior:

* If any of its children exits it will send SIGTERM to all the others.
* It will ignore SIGINT and SIGTERM but will forward them to all its children.
* It will only terminate when all its children have exited.
* If any of its children has exited in an abnormal way it will return -1.

Installation
------------

### Binary using glibc (Ubuntu, Debian, ...)

    wget https://github.com/nicolas-van/multirun/releases/download/0.3.0/multirun-ubuntu-0.3.0.tar.gz
    tar -zxvf multirun-ubuntu-0.3.0.tar.gz
    mv multirun /bin
    rm multirun-ubuntu-0.3.0.tar.gz
    
### Binary using musl libc (Alpine)

    wget https://github.com/nicolas-van/multirun/releases/download/0.3.0/multirun-alpine-0.3.0.tar.gz
    tar -zxvf multirun-alpine-0.3.0.tar.gz
    mv multirun /bin
    rm multirun-alpine-0.3.0.tar.gz
    
### Sources

    git clone --branch 0.3.0 https://github.com/nicolas-van/multirun.git
    cd multirun
    make
    cp multirun /bin
    cd ..
    rm -rf multirun
    
