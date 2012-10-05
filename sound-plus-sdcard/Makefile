# project config
MAIN    = main
OBJECTS = \
	main.o
#	a48to96.o \

# list needed files from the Microchip Application Library here:
#APPLIB_OBJECTS = 
#APPLIB = libmchpapp.a

# reserve space for heap in RAM
LDFLAGS = -Wl,--defsym,_min_heap_size=0x400

include ../default.mk
