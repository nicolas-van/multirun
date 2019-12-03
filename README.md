
# multirun

[![Build Status](https://cloud.drone.io/api/badges/nicolas-van/multirun/status.svg)](https://cloud.drone.io/nicolas-van/multirun)

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

### Alpine

#### Package manager

```
apk add multirun
```

#### Binary install

```
wget https://github.com/nicolas-van/multirun/releases/download/0.3.2/multirun-musl-0.3.2.tar.gz && \
tar -zxvf multirun-musl-0.3.2.tar.gz && \
mv multirun /bin && \
rm multirun-musl-0.3.2.tar.gz
```

### Ubuntu, Debian, Red Hat, Centos...

#### Binary install

```
wget https://github.com/nicolas-van/multirun/releases/download/0.3.2/multirun-glibc-0.3.2.tar.gz && \
tar -zxvf multirun-glibc-0.3.2.tar.gz && \
mv multirun /bin && \
rm multirun-glibc-0.3.2.tar.gz
```

### From sources

    git clone --branch 0.3.2 https://github.com/nicolas-van/multirun.git && \
    cd multirun && \
    make && \
    cp multirun /bin && \
    cd .. && \
    rm -rf multirun
   
## FAQ
   
### When to use multirun in a Docker container

*The Docker documentation and most best practices documents regarding Docker tells that you should embed no more than one application per container*. This is indeed perfectly true.

One container is meant to contain one application. If you want to deploy multiple applications at once (like a web server and a database) you should arrange your containers to properly communicate with the external world using network and volumes, make them easily configurable though environment variables and pack everything in a [docker-compose](https://docs.docker.com/compose/) or similar tool configuration. That is the correct way to do the job 95% of the time. It will allow you to keep each application separate from each other, to isolate their respective environments, to manage their lifecycle separately, to identify clearly which volume is dedicated to what, to scale all applications independently, etc...

Then there is the remaining 5% where it's hard to define precisely what an application is. An application is not just a single process (countless web servers and databases spawn multiple processes for performance reason) nor a number of processes doing the same thing (again, a lot of applications spawn different processes with different purposes). Multirun, or any process manager in general, should be considered in a Docker container only for those specific cases where you consider that a single application necessitate multiple processes to fulfill its single purpose.

Here are some good use cases where multirun can be useful:

* *You want to launch multiple homogeneous single-threaded processes to effectively use multiple CPUs (and, for some reason, scaling your containers doesn't fit you):* There often exist some technology-specific solutions for this but for simple needs multirun can be useful.
* *You need to deploy an old-school Unix-style multi-process "I've been coded in the 80s" (also known as "Those kids they have it too easy with all their threads") service where the same base code yields multiple executable files that must all run concurrently:* Multirun can be useful.
* *You have multiple heterogeneous processes that serve the same purpose, use the same filesystem and that filesystem isn't meant to be persisted (AKA you don't want to use volumes for that filesystem):* Multirun can be useful.

Here is an example of bad use case:

* *You want to pack a web server, a scheduler, a message queue, some workers and a database in a single container to make it "easy" to deploy:* No no no, you're doing it wrong. Go learn about docker-compose, modify a little bit your application to properly use environment variables for things like database address and credentials, create a proper documentation and example `docker-compose.yml` file and send it to persons that need to deploy your application. It may seem more complicated but it's not. On the long run you will be much more effective in your Docker usage.

## Contributing

[See the contribution guide.](CONTRIBUTING.md)

## License

[See the license.](LICENSE.md)
