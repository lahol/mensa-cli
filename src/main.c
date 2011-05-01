#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <ctype.h>
#include <string.h>
#include <locale.h>
#include <libintl.h>

#include "commands.h"
#include "defaults.h"
#include "cmd.h"
/*#include "libmensa/mensa.h"
#include "mensa-output.h"
#define _XOPEN_SOURCE
#include <time.h>*/

int parse_cmdline(int argc, char **argv);

int cmd_info(int argc, char **argv) {
  printf("Info\n");
  return 0;
}

int main(int argc, char **argv) {
/*  parse_cmdline(argc, argv);*/
  char *rcpath = NULL;
  setlocale(LC_ALL, "");
  
  defaults_add("cmd.default", "show");
  defaults_add("show.date", "today");
  defaults_add("show.schema", "");
  defaults_add("show.clear-cache", "no");
  defaults_add("show.header", "Offers for %A, %x");
  defaults_add("show.max-width", "80");
  
  rcpath = utils_get_rcpath();
  
  if (utils_file_exists(rcpath)) {
    if (defaults_read(rcpath) != 0) {
      defaults_free();
      if (rcpath) {
        free(rcpath);
      }
      return 1;
    }
  }

  command_add("help", cmd_help, NULL);
  command_add("info", cmd_info, NULL);
  command_add("show", cmd_show, NULL);
  command_add("config", cmd_config, NULL);
  
  command_call(argc, argv);
  
  if (rcpath) {
    free(rcpath);
  }

  defaults_free();
  
  return 0;
}

int parse_cmdline(int argc, char **argv) {
  int c;
  static int clear_cache_flag;
  int option_index = 0;
  static struct option long_options[] = {
    { "clear-cache", no_argument, &clear_cache_flag, 1 },
    { "date", optional_argument, 0, 'd' },
    { 0, 0, 0, 0}
  };
  
  while ((c = getopt_long(argc, argv, "d:c", 
                          long_options, &option_index)) != -1) {
    switch (c) {
      case 0:
        printf("option 0\n");
        break;
      case 'c':
        printf("option c\n");
        break;
      case 'd':
        printf("option d\n");
        break;
      case '?':
        printf("unknown\n");
        break;
      default:
        printf("default\n");
    }
  }
  if (optind < argc) {
    printf("non-option ARGV-elements: ");
    while (optind < argc) {
      printf("%s ", argv[optind++]);
    }
    putchar('\n');
  }
  return 0;
}

