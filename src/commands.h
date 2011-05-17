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
/** @defgroup commands Command handling.
 *  @ingroup mensacli
 *  @file
 *  @ingroup commands
 *  Header file for the main command handling.
 */
#ifndef __COMMANDS_H__
#define __COMMANDS_H__

/** @typedef CMD_CALLBACK 
 * Callback for a subcommand of mensa.
 */
typedef int (*CMD_CALLBACK)(int, char**);
/** @typedef CMD_HELP_CALLBACK
 *  Callback to print help text for a command.
 */
typedef void (*CMD_HELP_CALLBACK)(void);

int command_add(char *name, CMD_CALLBACK cb, 
  CMD_HELP_CALLBACK help_cb);
  
int command_call(int argc, char **argv);
int command_help(char *name);

#endif
