#!/bin/sh

docker build -t multirun-build:latest .
docker run -it --rm -v /tmp/dist:/dist multirun-build:latest
mv /tmp/dist/multirun .
tar zcvf multirun-x64-$1.tar.gz multirun
