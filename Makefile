# © 2015 Gordon Cichon

CFLAGS = -MMD -Wall -D_XOPEN_SOURCE -O2

LIB = libffppheap.a
OBJS = \
	FFPPHeap_delete.o \
	FFPPHeap_enter.o \
	FFPPHeap_new.o \
	FFPPHeap_print.o \
	FFPPHeap_printEntry.o \
	FFPPHeap_remove.o \
	FFPPHeap_top.o

.PHONY: all clean realclean test

all: ffppheap_tb test

ffppheap_tb: ffppheap_tb.o csv.o $(LIB)

test:
	./ffppheap_tb
	diff sorted-ref.csv sorted-res.csv

$(LIB): $(LIB)($(OBJS))

clean:
	-rm *.d *.o $(LIB)

realclean: clean
	-rm $(LIB) ffppheap_tb ffppheap_tb.exe sorted-res.csv

-include $(wildcard *.d)
