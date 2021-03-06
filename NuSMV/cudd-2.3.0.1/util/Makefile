# $Id: Makefile,v 1.1 2001/10/29 13:40:07 none Exp $
#
# util -- miscellaneous utility routines
#---------------------------------------------------------------------------
.SUFFIXES: .c .o .u

CC	= gcc
RANLIB	= ranlib

FLAGS	= -DUNIX
MFLAG   =
ICFLAGS = -g
CFLAGS  = $(ICFLAGS) $(MFLAG) $(XCFLAGS)

LINTFLAGS = -u -n

# this is to create the lint library
LINTSWITCH = -o

P	= util
PSRC	= cpu_time.c cpu_stats.c getopt.c safe_mem.c strsav.c texpand.c \
	  ptime.c prtime.c pipefork.c pathsearch.c stub.c \
	  tmpfile.c datalimit.c
#	  restart.c tmpfile.c datalimit.c

POBJ	= cpu_time.o cpu_stats.o getopt.o safe_mem.o strsav.o texpand.o \
	  ptime.o prtime.o pipefork.o pathsearch.o stub.o \
	  tmpfile.o datalimit.o
#	  restart.o tmpfile.o datalimit.o
PUBJ	= cpu_time.u cpu_stats.u getopt.u safe_mem.u strsav.u texpand.u \
	  ptime.u prtime.u pipefork.u pathsearch.u stub.u \
	  tmpfile.u datalimit.u
#	  restart.u tmpfile.u datalimit.u
PHDR	= util.h

WHERE	= ..
INCLUDE = $(WHERE)/include

lib$(P).a: $(POBJ)
	ar rv lib$(P).a $(POBJ)
	$(RANLIB) lib$(P).a

.c.o: $(PHDR)
	$(CC) -c $< -I$(INCLUDE) $(FLAGS) $(CFLAGS)

optimize_dec: lib$(P).b

lib$(P).b: $(PUBJ)
	ar rv lib$(P).b $(PUBJ)
	$(RANLIB) lib$(P).b

.c.u: $(PHDR)
	cc -j $< -I$(INCLUDE) $(FLAGS) $(XCFLAGS)

lint: llib-l$(P).ln

llib-l$(P).ln: $(PSRC) $(PHDR)
	lint $(LINTFLAGS) $(LINTSWITCH)$(P) -I$(INCLUDE) $(PSRC)

tags: $(PSRC) $(PHDR)
	ctags $(PSRC) $(PHDR)

all: lib$(P).a lib$(P).b llib-l$(P).ln tags

clean:
	rm -f *.o *.u core *.warnings

distclean: clean
	rm -f lib*.a lib$(P).b llib-l$(P).ln *.bak *~ tags .pure
