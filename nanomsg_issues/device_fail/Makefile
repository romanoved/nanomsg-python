#!/usr/bin/make -f

BIN_LIST := $(basename $(wildcard *.c))

all: $(BIN_LIST)

nanomsg:
	git clone https://github.com/nanomsg/nanomsg.git
	cd nanomsg && ./autogen.sh

nanomsg_build: nanomsg
	cd nanomsg \
	&& ./configure --prefix $(PWD)/$@ \
	&& make -j16 \
	&& make install

clean:
	-rm $(BIN_LIST)

test: $(BIN_LIST)
	make -j run_server run_client

run_%: %
	LD_LIBRARY_PATH=./nanomsg_build/lib/ ./$*

%: %.c _common.h nanomsg_build
	$(CC) -g $< -o $@ -Wall -Wno-unused-variable -Inanomsg_build/include -Lnanomsg_build/lib -lnanomsg -lrt -lpthread -lanl

