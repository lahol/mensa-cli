include Makefile.inc

DIRS = libmensa
OBJS = main.o defaults.o commands.o mensa-output.o
OBJLIBS = libmensa.a
CFLAGS = $(PRJFLAGS) -I. -I./libmensa 
LDFLAGS = -L./libmensa

all: mensa libmensa.a

mensa: $(OBJS) $(OBJLIBS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o mensa $(OBJS) $(LIBS) -lmensa `xml2-config --cflags --libs` -ltermcap

main.o: main.c defaults.h commands.h
	$(CC) $(CFLAGS) -c -o main.o main.c

defaults.o: defaults.c defaults.h
	$(CC) $(CFLAGS) -c -o defaults.o defaults.c

commands.o: commands.c commands.h defaults.h
	$(CC) $(CFLAGS) -c -o commands.o commands.c
	
mensa-output.o: mensa-output.c mensa-output.h
	$(CC) $(CFLAGS) -c -o mensa-output.o mensa-output.c
	
#libmensa.so: mensa.o
#	$(CC) $(CFLAGS) -shared -o libmensa.so mensa.o
libmensa.a: force_look
	cd libmensa; $(MAKE) $(MFLAGS)

clean:
	rm -f *.o mensa
	for d in $(DIRS); do (cd $$d; $(MAKE) clean ); done

force_look:
	true