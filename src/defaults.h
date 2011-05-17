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
/** @defgroup defaults Default settings and configuration.
 *  @ingroup mensacli
 *  @file
 *  @ingroup defaults
 *  Header file for default settings and configuration.
 */
#ifndef __DEFAULTS_H__
#define __DEFAULTS_H__

/** @enum DefaultsError
 *  @brief Error codes for the defaults module
 */
/* @{ */
typedef enum {
  DEFAULTS_ERROR_OK = 0,            /**< @brief No errors occured. */
  DEFAULTS_ERROR_TYPE_MISMATCH,     /**< @brief String cannot be converted to 
                                         desired type. */
  DEFAULTS_ERROR_NOTFOUND,          /**< @brief Did not find the specified key. */
} DefaultsError;
/* @} */

/** @brief Enumeration result. */
typedef struct _DefaultsEnumResult {
  int numResults;           /**< @brief The number of results. */
  unsigned char **keys;     /**< @brief An array of matching keys. */
  unsigned char **values;   /**< @brief An array of the associated values. */
} DefaultsEnumResult;

int defaults_read(unsigned char *filename);
int defaults_write(unsigned char *filename);
void defaults_free(void);

DefaultsError defaults_get(unsigned char *key, unsigned char **value);
DefaultsError defaults_get_int(unsigned char *key, int *value);
DefaultsError defaults_get_boolean(unsigned char *key, int *value);

void defaults_enum(unsigned char *prefix, DefaultsEnumResult *result);
void defaults_enum_result_free(DefaultsEnumResult *result);

void defaults_add(unsigned char *key, unsigned char *value);
void defaults_update(unsigned char *key, unsigned char *value);

#endif
