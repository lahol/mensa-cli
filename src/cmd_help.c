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
/** @file
 *  @ingroup commands
 *  Implementation of the help command.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cmd.h"
#include "mensa-output.h"
#include "utils.h"

/** Run the help command.
 *  @param[in] argc The number of command line arguments.
 *  @param[in] argv The command line arguments.
 *  @return 0 on success.
 */
int cmd_help(int argc, char **argv) {
  if (argc <= 2) {
    printf("Usage: %s [cmd [args]]\n", argv[0]);
    printf(
      "\nCommands: \n"
      "    help            Show this help. \n"
      "    info            Show information about this program. \n"
      "    show            Show meals. \n"
      "    config          Show configuration or set values. \n"
      "\n"    
      "Use %s help cmd to show help about this command.\n", argv[0]);
    return 0;
  }
  else if (argc > 2) {
    return command_help(argv[1]);
  } 
}
