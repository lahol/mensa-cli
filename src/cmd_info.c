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
 *  Implementation of the info command.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cmd.h"
#include "mensa-output.h"
#include "utils.h"

/** Run the info command. Output license information.
 *  @param[in] argc The number of command line arguments.
 *  @param[in] argv The command line arguments.
 *  @return 0 on success.
 */
int cmd_info(int argc, char **argv) {
  int term_width = mensa_output_get_term_width();
  if (term_width == -1) term_width = 80;
  mensa_output_block(stdout,
    "Mensa-CLI - a command line interface for libmensa",
    term_width, 0, 0);
  mensa_output_block(stdout,
    "Copyright (C) 2011, Holger Langenau",
    term_width, 0, 0);
  fputc('\n', stdout);
  mensa_output_block(stdout,
    "Mensa-CLI is free software: you can redistribute it and/or modify "
    "it under the terms of the GNU General Public License as published by "
    "the Free Software Foundation, either version 3 of the License, or "
    "(at your option) any later version.",
    term_width, 0, 0);
  fputc('\n', stdout);
  mensa_output_block(stdout,
    "Mensa-CLI is distributed in the hope that it will be useful, "
    "but WITHOUT ANY WARRANTY; without even the implied warranty of "
    "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the "
    "GNU General Public License for more details.",
    term_width, 0, 0);
  fputc('\n', stdout);
  mensa_output_block(stdout,
    "You should have received a copy of the GNU General Public License "
    "along with Mensa-CLI.  If not, see <http://www.gnu.org/licenses/>.",
    term_width, 0, 0);
  fputc('\n', stdout);
  return 0;
}
