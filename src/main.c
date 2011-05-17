/*  Mensa-CLI - a command line interface for libmensa
 *  Copyright (C) 2011, Holger Langenau
 *
 *  This file is part of Mensa-CLI.
 *
 *  Mensa-CLI is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 * 
 *  Mensa-CLI is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Mensa-CLI.  If not, see <http://www.gnu.org/licenses/>.
 */
/** @mainpage
 *  Mensa-CLI is a small program intended to show available meals of a
 *  preferred cafeteria (mensa from Latin `table', common name for cafeterias
 *  in German universities) in the terminal.
 *
 *  Provided that the information is available in some form of html/xml a
 *  schema can be used to retrieve the information.
 *
 *  At least one schema file should be specified via schema. \e name=\e path and
 *  activated via show.schema=\e name. This can be done in the file ~/.mensarc
 *  Mensa-CLI also provides a shared library `libmensa' which is responsible
 *  for all parsing.
 * 
 *  @version 0.1
 *  @date 2011-05-01
 *  First usable version.
 *
 *  @defgroup mensacli Command line interface for libmensa.
 *  @file
 *  @ingroup mensacli
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

/** @brief Run the info command.
 *  @param[in] argc The number of command line arguments.
 *  @param[in] argv The command line arguments.
 *  @return 0
 */
int cmd_info(int argc, char **argv) {
  printf("Info\n");
  return 0;
}

/** @brief Entry point for the program.
 *  Initialize all subsystems, invoke the commands and clean up.
 *  @param[in] argc The number of command line arguments.
 *  @param[in] argv The command line arguments.
 *  @return 0 on success, 1 otherwise.
 */
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

