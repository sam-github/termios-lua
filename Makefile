.PHONY: default build test

-include local.mak

default: build

UNAME=$(shell uname)

include $(UNAME).mak

BINDING=termios.so

build: $(BINDING)

prefix=/usr/local

SODIR = $(DESTDIR)/$(prefix)/lib/lua/5.1/

.PHONY: install
install: $(BINDING)
	mkdir -p $(SODIR)
	install -t $(SODIR) $(BINDING)

CWARNS = -Wall \
  -pedantic \
  -Wcast-align \
  -Wnested-externs \
  -Wpointer-arith \
  -Wwrite-strings

COPT=-O2 -DNDEBUG
CFLAGS=$(CWARNS) $(CDEFS) $(CLUA) $(LDFLAGS)
LDLIBS=$(LLUA)

CC.SO := $(CC) $(COPT) $(CFLAGS)

%.so: %.c
	$(CC.SO) -o $@ $^ $(LDLIBS)

termios.so: termios.c

doc: README.txt

.PHONY: README.txt
README.txt: termios.c
	luadoc termios.c > $@

