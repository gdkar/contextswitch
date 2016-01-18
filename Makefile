CC = gcc
CFLAGS = -march=native -g -ggdb -O3 -Ofast -D_XOPEN_SOURCE=600 -D_GNU_SOURCE -std=c11 -Wall -Werror -Wextra -pthread -fpic 
LDFLAGS = -lrt -pthread

LIBFLAGS = -shared

all: build
timelibcall: liblib.so
timelibcall: LDFLAGS += -L./ -llib
TARGETS = liblib.so timectxswsem timectxswpipe timectxsw timectxswws timelibcall timespin timefncall timesyscall timetctxsw timetctxsw2 timetctxswpipe timetctxswsem timetctxswcond timemutex timesem timecond



lib%.so: %.c
	$(CC) $(CFLAGS)  $(LIBFLAGS) -o $@ $<


build: $(TARGETS)
bench: $(TARGETS)
	sudo -E ./cpubench.sh

clean:
	rm -f $(TARGETS)

.PHONY: all bench build
