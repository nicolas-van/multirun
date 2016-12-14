from ubuntu

run apt-get -y update
run apt-get -y install gcc make
add . /root/multirun
run rm /root/multirun/multirun

volume ["/dist"]

cmd sh -c "cd /root/multirun; make; cp multirun /dist"
