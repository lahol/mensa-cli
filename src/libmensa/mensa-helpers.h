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
/** @defgroup libhelper Helper functions for libmensa.
 *  @ingroup libmensa
 *  @file
 *  @ingroup libhelper
 *  Header file for helper functions.
 */
#ifndef __MENSA_HELPERS_H__
#define __MENSA_HELPERS_H__

/** @brief List element for single linked list
 */
/* @{ */
typedef struct _mensaList mensaList;
struct _mensaList {
  void *data;            /**< @brief Pointer to the data stored. */
  mensaList *next;       /**< @brief Pointer to the next element in the list. */
};
/* @} */

mensaList *mensa_list_prepend(mensaList *list, void *data);
mensaList *mensa_list_reverse(mensaList *list);

void mensa_string_beautify(char *str, int inner);

#endif