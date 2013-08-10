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
/** @defgroup utils Utilities
 *  @ingroup mensacli
 *  @file
 *  @ingroup utils
 *  Header file for utilities used by the command line interface.
 */
#ifndef __UTILS_H__
#define __UTILS_H__

int utils_file_exists(const char *filename);
char * utils_get_rcpath(void);

/** @brief Single linked list.
 */
/* @{ */
typedef struct _UtilsList UtilsList;
struct _UtilsList {
  void *data;          /**< @brief Pointer to the data stored in the list. */
  UtilsList *next;     /**< @brief Pointer to the next list element. */
};
/* @} */

UtilsList *utils_list_prepend(UtilsList *list, void *data);

#endif
