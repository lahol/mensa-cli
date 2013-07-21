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
 *  @ingroup output
 *  Implementation of the output utilities.
 */
#include "mensa-output.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../config.h"

#if HAVE_TERM_H && HAVE_LIBTERMCAP
#include <curses.h>
#include <term.h>
#ifdef unix
static char term_buffer[2048];
#else
#define term_buffer 0
#endif
#endif

/** Print a string with a minimal width, take care of UTF-8 characters
 *  @param[in] stream The stream to write to, opened for writing. If this
 *                    NULL is specified, stdout is used.
 *  @param[in] str Pointer to the string or NULL.
 *  @param[in] len Length of the string.
 */
void mensa_output_fixed_len_str(FILE *stream, char *str, int len) {
  int k = 0, l = 0;
  FILE *s = stream ? stream : stdout;
  if (!str) {
    for (k = 0; k < len; k++) {
      fputc(' ', s);
    }
    return;
  }
  while (str[k] != '\0') {
    if ((str[k] & 0x80) == 0) {
      fputc(str[k++], s);
    }
    else if ((str[k] & 0xe0) == 0xc0) {
      fputc(str[k++], s);
      fputc(str[k++], s);
    }
    else if ((str[k] & 0xf0) == 0xe0) {
      fputc(str[k++], s);
      fputc(str[k++], s);
      fputc(str[k++], s);
    }
    else if ((str[k] & 0xf8) == 0xf0) {
      fputc(str[k++], s);
      fputc(str[k++], s);
      fputc(str[k++], s);
      fputc(str[k++], s);
    }
    l++;
  }
  for (; l < len; l++) {
    fputc(' ', stream);
  }
}

int _mensa_output_block_line(FILE *stream, char *str, int length);

/** Output a block of text of fixed line width. Break lines at
 *  whitespaces, commas, periods, semicolons, colons.
 *  @param[in] stream The file to write to, open for writing. If
 *                    NULL is given, stdout is used.
 *  @param[in] str The string for output.
 *  @param[in] length Maximal length of a line.
 *  @param[in] indent Number of characters to indent the block.
 *  @param[in] indent_first_line >0 if the first line should be indented,
 *                               <0 shorten first line (e.g. if label is
 *                               longer than indent)
 */
void mensa_output_block(FILE *stream, char *str, int length,
                        int indent, int indent_first_line) {
  int offset = 0;
  int delta = 0;  
  int len = 0;
  FILE *s = stream ? stream : stdout;
  int k;
  
  if (!str) {  /* no string given, just print newline */
    fputc('\n', s);
    return;
  }
  len = strlen(str);
  while (offset < len) {
    if (offset > 0 || (offset == 0 && indent_first_line > 0)) {
      for (k = 0; k < indent; k++) {
        fputc(' ', s);
      }
    }
    delta = _mensa_output_block_line(s, &str[offset],
                length + (offset == 0 && indent_first_line < 0 ? indent_first_line : 0));
    if (delta == 0) {
      break;
    }
    offset += delta;
  }
}

/** Writes as much of a line as possible to a file.
 *  @param[in] stream The file to write to or NULL. If NULL is given
 *                    stdout is used.
 *  @param[in] str The string buffer from which the line is read.
 *  @param[in] length The maximal number of characters to print.
 *  @return offset of the next line 
 *  @internal
 */
int _mensa_output_block_line(FILE *stream, char *str, int length) {
  int len;
  int i,d;
  int last_space;
  int offset = 0;
  FILE *s = stream ? stream : stdout;
  
  /* skip leading spaces but be aware of the complete offset; needs work */
  while (str[offset] == ' ') { offset++; }
  if (offset) str = &str[offset];

  len = strlen(str);
  if (len <= length) { /* nothing to cut of, write to output */
    for (i = 0; str[i] != '\0' && str[i] != '\n'; i++) {
      fputc(str[i], s);
    }
    fputc('\n', s);
    return i+offset;
  }
  
  i = 0; d = 0;
  last_space = 0;
  /* could be done simpler, but we want to take care of utf-8 characters */
  while (i < length && str[i+d] != '\0') {
    if (str[i+d] == ' ' || str[i+d] == ',' || str[i+d] == '-' ||
        str[i+d] == ';' || 
         (str[i+d] == '.' && (str[i+d+1] != ',' && str[i+d+1] != ';'
          && str[i+d+1] != '-'))
        || str[i+d] == ':') {
      last_space = i+d;
    }
    else if (str[i+d] == '\n') {
      last_space = i+d;
      break;
    }
    else if ((str[i+d] & 0xe0) == 0xc0) {
      d++;
    }
    else if ((str[i+d] & 0xf0) == 0xe0) {
      d += 2;
    }
    else if ((str[i+d] & 0xf8) == 0xf0) {
      d+= 3;
    }
    i++;
  }
  if (last_space == 0) { /* no spaces up to length, cut of */
    last_space = i+d-1; /* -1 ? */
  }

  for (i = 0; i <= last_space; i++) {
    fputc(str[i], s);
  }
  fputc('\n', s);
  return last_space+1+offset;
}

/** Get the terminal width
 *  @return The width of the terminal or -1 if determination of
 *          width is not possible.
 */
int mensa_output_get_term_width(void) {
#if HAVE_TERM_H && HAVE_LIBTERMCAP
  char *termtype = getenv("TERM");
  if (termtype == NULL) {
    return -1;
  }
  
  if (tgetent(term_buffer, termtype) <= 0) {
    return -1;
  }
  
  return tgetnum("co");
#else
  return -1;
#endif
}
