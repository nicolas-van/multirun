#!/bin/sh

rm multirun
cp Dockerfile.ubuntu Dockerfile
docker build -t multirun-build-ubuntu:latest .
docker run -it --rm -v /tmp/dist:/dist multirun-build-ubuntu:latest
mv /tmp/dist/multirun .
tar zcvf multirun-ubuntu-$1.tar.gz multirun
rm Dockerfile

rm multirun
cp Dockerfile.alpine Dockerfile
docker build -t multirun-build-alpine:latest .
docker run -it --rm -v /tmp/dist:/dist multirun-build-alpine:latest
mv /tmp/dist/multirun .
tar zcvf multirun-alpine-$1.tar.gz multirun
rm Dockerfile

rm multirun
