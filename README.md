
multirun
========

A simple Unix utility in C to run multiple commands concurrently. It is mostly aimed as a much lighter alternative to full-fledged init processes or supervisord to run multiple servers in the same Docker container.

Usage: `multirun "command1" "command2" ...`

You can also add the `-v` option to get a full log of the processes it starts and kills.

multirun never attempts to restart one of its children if it exists. Instead it will kill all its other children before exiting itself. This behavior is ideal for a Docker container as the restart behavior can be delegated to the upper level, as example using systemd or Docker restart policies. Also it forwards all stdout and stderr outputs to be properly logged using Docker's logging features.

Installation
------------

### Binary using glibc (Ubuntu, Debian, Red Hat, Centos...)

    wget https://github.com/nicolas-van/multirun/releases/download/0.3.0/multirun-ubuntu-0.3.0.tar.gz && \
    tar -zxvf multirun-ubuntu-0.3.0.tar.gz && \
    mv multirun /bin && \
    rm multirun-ubuntu-0.3.0.tar.gz
    
### Binary using musl libc (Alpine)

    wget https://github.com/nicolas-van/multirun/releases/download/0.3.0/multirun-alpine-0.3.0.tar.gz && \
    tar -zxvf multirun-alpine-0.3.0.tar.gz && \
    mv multirun /bin && \
    rm multirun-alpine-0.3.0.tar.gz
    
### Sources

    git clone --branch 0.3.0 https://github.com/nicolas-van/multirun.git && \
    cd multirun && \
    make && \
    cp multirun /bin && \
    cd .. && \
    rm -rf multirun
    
