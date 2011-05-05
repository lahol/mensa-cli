SUBDIRS = libmensa

bin_PROGRAMS = mensa
mensa_SOURCES = main.c defaults.c commands.c mensa-output.c \
                defaults.h commands.h mensa-output.h cmd.h \
                cmd_show.c cmd_config.c cmd_help.c utils.h utils.c
mensa_LDADD = libmensa/libmensa.la
mensa_LDFLAGS = -rpath $(libdir)