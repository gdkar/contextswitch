CC = gcc
CFLAGS = -march=native -g -ggdb -O3 -Ofast -D_XOPEN_SOURCE=600 -D_GNU_SOURCE -std=c11 \
         -Wall -Werror -Wextra -pthread -fpic 
LDFLAGS = -lrt -lpthread

LIBFLAGS = -shared

TARGETS = liblib.so timectxsw timectxswws timelibcall timespin timefncall timesyscall timetctxsw timetctxsw2

all: bench

timelibcall: liblib.so
timelibcall: LDFLAGS += -L./ -llib


lib%.so: %.c
	$(CC) $(CFLAGS)  $(LIBFLAGS) -o $@ $<


bench: $(TARGETS)
	./cpubench.sh

clean:
	rm -f $(TARGETS)

.PHONY: all bench
