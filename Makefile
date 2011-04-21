CC = gcc
AR = ar
ARFLAGS =
RANLIB = ranlib
CFLAGS = -Wall

all: mensa libmensa.a

mensa: main.o defaults.o commands.o libmensa.a
	$(CC) $(CFLAGS) -o mensa main.o defaults.o commands.o -L . -lmensa

main.o: main.c defaults.h commands.h
	$(CC) $(CFLAGS) -c -o main.o main.c

defaults.o: defaults.c defaults.h
	$(CC) $(CFLAGS) -c -o defaults.o defaults.c

commands.o: commands.c commands.h defaults.h
	$(CC) $(CFLAGS) -c -o commands.o commands.c
	
#libmensa.so: mensa.o
#	$(CC) $(CFLAGS) -shared -o libmensa.so mensa.o
libmensa.a: mensa.o
	$(AR) $(ARFLAGS) rv libmensa.a mensa.o
	$(RANLIB) libmensa.a

mensa.o: mensa.c mensa.h
#	$(CC) $(CFLAGS) -c -fpic -o mensa.o mensa.c
	$(CC) $(CFLAGS) -c -o mensa.o mensa.c


clean:
	rm -f *.o mensa libmensa.so libmensa.a
