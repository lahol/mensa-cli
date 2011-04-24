include ../Makefile.inc

CFLAGS = $(PRJCFLAGS) `xml2-config --cflags`
OBJLIBS = libmensa.a

OBJS = mensa.o mensaschema.o

all: $(OBJLIBS)

libmensa.a: $(OBJS)
	$(AR) $(ARFLAGS) rv libmensa.a $(OBJS)
	$(RANLIB) libmensa.a

mensa.o: mensa.c mensa.h mensaschema.h
	$(CC) $(CFLAGS) -c -o mensa.o mensa.c
	
mensaschema.o: mensaschema.c mensaschema.h
	$(CC) $(CFLAGS) -c -o mensaschema.o mensaschema.c
	
clean: 
	$(RM) -f *.o libmensa.a