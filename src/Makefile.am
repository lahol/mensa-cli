SUBDIRS = libmensa

bin_PROGRAMS = mensa
mensa_SOURCES = main.c defaults.c commands.c mensa-output.c \
                defaults.h commands.h mensa-output.h cmd.h \
                cmd_show.c cmd_config.c cmd_help.c cmd_info.c \
                utils.h utils.c
mensa_LDADD = libmensa/libmensa.la ${CURL_LIBS}
# -R was -rpath
mensa_LDFLAGS = -R $(libdir)
