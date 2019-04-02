#!/bin/sh

rm -f multirun
docker build -t multirun-build-ubuntu:latest -f Dockerfile.ubuntu .
docker run -it --rm -v /tmp/dist:/dist multirun-build-ubuntu:latest
mv /tmp/dist/multirun .
tar czvf multirun-ubuntu-$1.tar.gz multirun

rm -f multirun
docker build -t multirun-build-alpine:latest -f Dockerfile.alpine .
docker run -it --rm -v /tmp/dist:/dist multirun-build-alpine:latest
mv /tmp/dist/multirun .
tar czvf multirun-alpine-$1.tar.gz multirun

rm -f multirun
