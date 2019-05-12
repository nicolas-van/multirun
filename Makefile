PREFIX := /usr/local
CC := gcc
CFLAGS := -std=gnu11 -O3 -static -no-pie -pipe -fstack-protector-strong -fPIC -fno-plt -fomit-frame-pointer
CPPFLAGS := -DFORTIFY_SOURCE=2
LDFLAGS := -static -Wl,-O1,--sort-common,--as-needed,-z,relro,-z,now,--hash-style=gnu,-Bstatic

multirun: multirun.c
	$(CC) -c -o $@.o $(CFLAGS) $^
	$(CC) -o $@ $(LDFLAGS) $@.o

test: multirun
	bats tests.bats

.PHONY: clean
clean:
	-rm multirun
	-rm multirun.o

.PHONY: install
install: multirun
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	cp $< $(DESTDIR)$(PREFIX)/bin/multirun

.PHONY: uninstall
uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/multirun
