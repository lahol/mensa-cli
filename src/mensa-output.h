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
/** @defgroup output Handle output to terminal.
 *  @ingroup mensacli
 *  @file
 *  @ingroup output
 *  Header file for output to terminal.
 */
#ifndef __MENSA_OUTPUT_H__
#define __MENSA_OUTPUT_H__

#include <stdio.h>

void mensa_output_fixed_len_str(FILE *stream, char *str, int len);

void mensa_output_block(FILE *stream, char *str, int length,
                        int indent, int indent_first_line);
                        
int mensa_output_get_term_width(void);

#endif
