/*  Libmensa - get information of available meals for a given canteen.
 *  Copyright (c) 2011, Holger Langenau
 *
 *  This file is part of Libmensa.
 *
 *  Libmensa is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 * 
 *  Libmensa is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Libmensa.  If not, see <http://www.gnu.org/licenses/>.
 */
/** @file
 *  @ingroup libhelpers
 *  Implementation of helper functions for libmensa.
 */
#include "mensa-helpers.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>

/** @brief Prepend list with new data element.
 *  @param[in] list The list to be prepended or NULL if list is empty.
 *  @param[in] data The data to store.
 *  @return A pointer to the new beginning of the list.
 */
mensaList *mensa_list_prepend(mensaList *list, void *data) {
  mensaList *ins = malloc(sizeof(mensaList));
  assert(ins);
  ins->next = list;
  ins->data = data;
  return ins;
}

/** @brief Reverse order of elements in list.
 *
 *  @note This function doesn't do anything at the moment. Since all
 *        lists are only used temporarily at the moment as their data
 *        is mapped to arrays when it is returned (if at all), the
 *        order of the elements doesn't really matter.
 *
 *  @param[in] list The list to reverse.
 *  @return The new head of the list.
 *  @todo
 */
mensaList *mensa_list_reverse(mensaList *list) {
  return list;
}

char _mensa_next_nonws(char *str, int *offset);

/** @brief Remove dispensable characters in string.
 *  Removes leading and trailing whitespaces; double whitespaces
 *  and whitespaces in front of ,.; if inner is true.
 *
 *  @note Non-break space (U+00A0) and CR and LF are considered
 *        to be a whitespace.
 *
 *  @param[in] str The string to beatify. Note that this operation
 *                 is in place, thus the string will be modified.
 *  @param[in] inner 1 if inner whitespaces and should be mapped
 *                   to a single space (0x20) or are removed
 *                   completly if they are in front of commas,
 *                   periods, semicolons or colons. 0 if not.
 */
void mensa_string_beautify(char *str, int inner) {
  char *buffer = NULL;
  int len, i, d;
  int spaceflag;
  char tok;
  int t;
  
  if (!str) {
    return;
  }
  /* remove trailing whitespaces */  
  len = strlen(str);
  for (i = len-1; i >= 0; i--) {
    if (str[i] == ' ' ||
        str[i] == '\t' ||
        str[i] == 0xd ||
        str[i] == 0xa) {
      str[i] = '\0';
    }
    else if (str[i] == (char)0xa && i > 0 && buffer[i-1] == (char)0xc2) {
      /* no-break space, U+00A0 (0xc2 0xa0)
       * needs type conversion because a0 < 0 for signed char */
      str[i] = str[i-1] = '\0';
      i--;
    }
    else {
      break;
    }
  }
  /* remove leading whitespaces */
  d = 0;
  do {
    if (str[d] != ' ' &&
        str[d] != '\t' &&
        str[d] != 0xa &&
        str[d] != 0xd) {
      if (str[d] != (char)0xc2 || str[d+1] != (char)0xa0) {
        break;
      }
      else {
       d++;
      }
    }
    d++;
  } while (str[d] != '\0');
  
  /* move string */
  if (d > 0) {
    i = 0;
    while (str[i+d] != '\0') {
      str[i] = str[i+d];
      i++;
    }
    str[i] = '\0';
  }
  
  if (!inner) {
    return;
  }
  
  /* do the inner stuff */
  /* copy buffer and write to str */
  len = strlen(str);
  buffer = strdup(str);
  if (!buffer) return;
  i = 0;
  d = 0;
  spaceflag = 0;
  while (buffer[i+d] != '\0') {
    if (buffer[i+d] == ' ' ||
        buffer[i+d] == '\t' ||
        buffer[i+d] == 0xa ||
        buffer[i+d] == 0xd) {
      tok = _mensa_next_nonws(&buffer[i+d], &t);
      if (tok == ',' || tok == '.' || tok == ';' || tok == ':') {
        d += t;
      }
      else if (!spaceflag) {
        str[i] = ' ';
        spaceflag = 1;
        i++;
      }
      else {
        d++;
      }
    }
    else if (buffer[i+d] == 0xc2 && buffer[i+d+1] == 0xa0) {
      tok = _mensa_next_nonws(&buffer[i+d], &t);
      if (tok == ',' || tok == '.' || tok == ';' || tok == ':') {
        d += t;
      }
      else if (!spaceflag) {
        str[i] = ' ';
        spaceflag = 1;
        i++; d++;
      }
      else {
        d += 2;
      }
    }
    else {
      spaceflag = 0;
      str[i] = buffer[i+d];
      i++;
    }
  }
  str[i] = '\0';
}

/** Find the next character that is not a whitespace.
 *  @param[in] str The string to find the next non-whitespace in.
 *  @param[out] offset The offset of the found character.
 *  @return The character found.
 *  @internal
 */
char _mensa_next_nonws(char *str, int *offset) {
  int k = 0;
  if (!str) return 0;
  while (str[k] != '\0') {
    if (str[k] != ' ' &&
        str[k] != '\t' &&
        str[k] != 0xa &&
        str[k] != 0xd) {
      if (str[k] != 0xc2 || str[k+1] != 0xa0) {
        if (offset) *offset = k;
        return str[k];
      }
      else {
        k++;
      }
    }
    k++;
  }
  if (offset) *offset = k;
  return 0;
}
