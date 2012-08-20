CC?=gcc
CFLAGS?=-g -O0
LIBS:=
#LDFLAGS=-static
CFLAGS+=-Wall
#CFLAGS+=-Wformat -Wformat-security -Wextra -Wunused -Wshadow -Wmissing-prototypes -Wcast-qual

# Get the proper include/lib searchpath & library name from the pkgconfig db
CPPFLAGS+=$(shell pkg-config --cflags-only-I libusb-1.0)
LDFLAGS+=$(shell pkg-config --libs-only-L libusb-1.0)
LIBS+=$(shell pkg-config --libs-only-l libusb-1.0)

all: usbtool

usbtool: usbtool.c
	$(CC) $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) $< -o $@ $(LIBS)

clean:
	rm -rf usbtool

.PHONY: all clean
