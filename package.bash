#!/bin/bash

rm -f multirun
cp Dockerfile.ubuntu Dockerfile
docker build -t multirun-build-ubuntu:latest .
mkdir -p dist
docker run -it --rm -v $(pwd)/dist:/dist multirun-build-ubuntu:latest
cp dist/multirun multirun
rm -rf dist
tar zcvf multirun-ubuntu-$1.tar.gz multirun
rm Dockerfile

rm -f multirun
cp Dockerfile.alpine Dockerfile
docker build -t multirun-build-alpine:latest .
mkdir -p dist
docker run -it --rm -v $(pwd)/dist:/dist multirun-build-alpine:latest
cp dist/multirun multirun
rm -rf dist
tar zcvf multirun-alpine-$1.tar.gz multirun
rm Dockerfile

rm multirun
