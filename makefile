CC=gcc
CFLAGS=-DKXVER=3 -DLUA_COMPAT_MODULE=1 -fpic -I/usr/local/include/lua5.2
LDFLAGS=-shared
LIBS=-L/usr/local/lib -llua
OUTPUT=k.so

makefile=$(shell uname -s | tr A-Z a-z).makefile
-include $(makefile)

$(OUTPUT): k.o kx/$(KARCH)/c.o makefile $(makefile); $(CC) $(CFLAGS) $(LDFLAGS) -o $(OUTPUT) k.o kx/$(KARCH)/c.o $(LIBS)
k.o: k.c kx/k.h makefile $(makefile)
kx/k.h kx/$(KARCH)/c.o makefile $(makefile):; sh kx.sh

clean:; rm -f k.so
test: $(OUTPUT) test.lua makefile; lua test.lua
