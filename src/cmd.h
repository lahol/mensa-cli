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
/** @ingroup commands
 *  @file
 *  Header file for the implementation of the supported commands and
 *  shared features.
 */
#ifndef __CMD_H__
#define __CMD_H__

int cmd_config(int argc, char **argv);
int cmd_config_set(char *arg);

int cmd_show(int argc, char **argv);
int cmd_help(int argc, char **argv);
int cmd_info(int argc, char **argv);

#endif