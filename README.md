
multirun
========

A simple Unix utility in C to run multiple commands concurrently. It is mostly aimed as a much lighter alternative to
supervisord to run multiple servers in the same Docker container.

Usage: `multirun "command1" "command2" ...`

You can also add the `-v` option to get a full log of the processes it starts and kill.

A few comments about its behavior:

* If any of its children exits it will send SIGTERM to all the others.
* It will ignore SIGINT and SIGTERM but will forward them to all its children.
* It will only terminate when all its children have exited.
* If any of its children has exited in an abnormal way it will return -1.
* It forwards the stdout and stderr of its children.

This behavior is ideal for Docker as multirun doesn't try to restart the servers it launches, it simply kills everything if any of its children exits. The process supervision job can then be delegated to systemd or Docker (using its restart policies). Those tools are better and more appropriate for that job than a supervisord instance running in a Docker container, notably to get centralized logs.

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
    
