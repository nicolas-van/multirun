
# multirun

[![Build](https://github.com/nicolas-van/multirun/actions/workflows/build.yml/badge.svg)](https://github.com/nicolas-van/multirun/actions/workflows/build.yml)

A simple Unix utility in C to run multiple commands concurrently.

* A very light alternative to classic init processes or supervisord to run multiple services in the same Docker container.
* Is dead-simple to use.
* Can be run without root permissions.
* Cleanly kills all the processes it starts.
* Delegates the restart duty to the upper level.
* Forwards stdout and stderr for proper logging with Docker.

Usage: `multirun "command1" "command2" ...`

You can also add the `-v` option to get a full log of the processes it starts and kills.

Unlink most process managers multirun never attempts to restart one of its children if it crashes. Instead it will kill all its other children before exiting with an error code. This behavior is ideal when you just want to delegate the restart duty to the upper level, as example using systemd or Docker restart policies.

## Installation

### Package manager

Currently, multirun is only present in Alpine's distribution

#### Alpine


```bash
apk add multirun
```

### Binary install

The [release page](https://github.com/nicolas-van/multirun/releases) lists all provided binaries.

Here is an example install script for an x86_64 Linux using GNU libc. Replace the URL with another one if you use another system.

```bash
wget -c https://github.com/nicolas-van/multirun/releases/download/1.0.1/multirun-x86_64-linux-gnu-1.0.1.tar.gz -O - | tar -xz && \
mv multirun /bin
```

We provide binaries for the following systems:

* x86_64 Linux, both using glibc (for most systems) and musl libc (for Alpine containers)
* aarch64 Linux (for cheaper servers like AWS EC2 t4g line), both using glibc and musl libc
* arm Linux (for Raspberry Pi), both using glibc and musl libc
* x86_64 Mac OS X

### From sources

This project necessitates CMake.

```bash
wget -c https://github.com/nicolas-van/multirun/archive/refs/tags/1.0.1.tar.gz -O - | tar -xz && \
mv multirun-1.0.1 multirun-src && \
cd multirun-src && \
cmake . && \
cmake --build . && \
cp multirun /bin && \
cd .. && \
rm -rf multirun-src
```
   
## FAQ
   
### When to use multirun in a Docker container

*The Docker documentation and most best practices documents regarding Docker tells that you should embed no more than one application per container*. This is indeed perfectly true.

One container is meant to contain one application. If you want to deploy multiple applications at once (like a web server and a database) you should arrange your containers to properly communicate with the external world using network and volumes, make them easily configurable though environment variables and pack everything in a [docker-compose](https://docs.docker.com/compose/) or similar tool configuration. That is the correct way to do the job 95% of the time. It will allow you to keep each application separate from each other, to isolate their respective environments, to manage their lifecycle separately, to identify clearly which volume is dedicated to what, to scale all applications independently, etc...

Then there is the remaining 5% where it's hard to define precisely what an application is. An application is not just a single process (countless web servers and databases spawn multiple processes for performance reason) nor a number of processes doing the same thing (again, a lot of applications spawn different processes with different purposes). Multirun, or any process manager in general, should be considered in a Docker container only for those specific cases where you consider that a single application necessitate multiple processes to fulfill its single purpose.

Here are some good use cases where multirun can be useful:

* You want to launch multiple homogeneous single-threaded processes to effectively use multiple CPUs (and, for some reason, scaling your containers doesn't fit you): There often exist some technology-specific solutions for this but for simple needs multirun can be useful.
* You need to deploy an old-school Unix-style multi-process *"I've been coded in the 80s"* *"Those kids, they have it too easy with all their threads and their garbage collectors!"* service where the same base code yields multiple executable files that must all run concurrently: Multirun can be useful.
* You have multiple heterogeneous processes that serve the same purpose, use the same filesystem and that filesystem isn't meant to be persisted (AKA you don't want to use volumes for that filesystem): Multirun can be useful.

Here is an example of bad use case:

* You want to pack a web server, a scheduler, a message queue, some workers and a database in a single container to make it "easy" to deploy: No no no, you're doing it wrong. Go learn about docker-compose, modify a little bit your application to properly use environment variables for things like database address and credentials, create a proper documentation, split everything into separate containers in an example `docker-compose.yml` file and send it to the persons that need to deploy your application. It may seem more complicated but it's not. On the long run you will be much more effective in your Docker usage.

### My processes do not exit correctly

A common cause for these problems is invalid usage of shell scripts that causes signals to not be propagated properly.

If you call multirun in a shell script, check that you launch it with the `exec` sh command. Example:

```bash
#!/bin/sh
# any init code

exec multirun arg1 arg2
```

This advice is not specific to multirun and does apply to most containers that have a shell script as entrypoint.

Also, if you launch scripts with multirun that will launch the service you want, be sure to add `exec` in these scripts as well. Example:

```bash
# multirun call
multirun ./service1.sh ./service2.sh
```

```bash
# ./service1.sh
#!/bin/sh

# any init code

exec service1_executable
```

```bash
# ./service2.sh
#!/bin/sh

# any init code

exec service2_executable
```

If it still doesn't work launch multirun in verbose mode and try to understand what's going on. Your problem is probably caused by a child process that doesn't exit properly or not fast enough when it receives a signal.

## Contributing

[See the contribution guide.](CONTRIBUTING.md)

## License

[See the license.](LICENSE.md)
