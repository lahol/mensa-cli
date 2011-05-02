/** @mainpage
 *  Mensa-CLI is a small program intended to show available meals of a
 *  preferred cafeteria (mensa from Latin `table', common name for cafeterias
 *  in German universities) in the terminal.
 *
 *  Provided that the information is available in some form of html/xml a
 *  schema can be used to retrieve the information.
 *
 *  At least one schema file should be specified via schema.<name>=<path> and
 *  activated via show.schema=<name>. This can be done in the file ~/.mensarc
 *  Mensa-CLI also provides a shared library `libmensa' which is responsible
 *  for all parsing.
 * 
 *  @version 0.1
 *  @date 2011-05-01
 *  First usable version.
 *
 *  @defgroup mensa-cli Command line interface for libmensa.
 *  @file
 *  The main file for the program.
 */
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

int cmd_info(int argc, char **argv) {
  printf("Info\n");
  return 0;
}

int main(int argc, char **argv) {
  char *rcpath = NULL;
  setlocale(LC_ALL, "");
  
  defaults_add("cmd.default", "show");
  defaults_add("show.date", "today");
  defaults_add("show.description", "yes");
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

