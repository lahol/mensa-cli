include ../Makefile.inc

CFLAGS = $(PRJCFLAGS) `xml2-config --cflags`
OBJLIBS = libmensa.a

OBJS = mensa.o mensa-schema.o mensa-helpers.o mensa-time.o

all: $(OBJLIBS)

libmensa.a: $(OBJS)
	$(AR) $(ARFLAGS) rv libmensa.a $(OBJS)
	$(RANLIB) libmensa.a

mensa.o: mensa.c mensa.h mensa-schema.h mensa-data.h mensa-time.h
	$(CC) $(CFLAGS) -c -o mensa.o mensa.c
	
mensa-schema.o: mensa-schema.c mensa-schema.h mensa-data.h
	$(CC) $(CFLAGS) -c -o mensa-schema.o mensa-schema.c

mensa-helpers.o: mensa-helpers.c mensa-helpers.h
	$(CC) $(CFLAGS) -c -o mensa-helpers.o mensa-helpers.c
	
mensa-time.o: mensa-time.c mensa-time.h
	$(CC) $(CFLAGS) -c -o mensa-time.o mensa-time.c
	
clean: 
	$(RM) -f *.o libmensa.a