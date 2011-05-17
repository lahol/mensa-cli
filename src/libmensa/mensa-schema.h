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
/** @defgroup schema Schema handling of the library.
 *  @ingroup libmensa
 *  @file
 *  @ingroup schema
 *  @brief Header file for schema handling.
 *
 *  A schema is a collection of information about the data to be retrieved.
 *  It contains information which html/xml-file it should load and where it
 *  finds the appropriate data for a day (using XPath). Furthermore information
 *  how to get the category of the meals.
 */
#ifndef __MENSASCHEMA_H__
#define __MENSASCHEMA_H__

#include "mensa-data.h"
#include "mensa-time.h"

typedef struct _mensaSchema mensaSchema;

mensaSchema * mensa_schema_read_from_file(const char *filename);
void mensa_schema_free(mensaSchema *schema);

void mensa_schema_get_description(mensaSchema *schema, unsigned char **desc);

MensaMealGroup * mensa_schema_get_foods(mensaSchema *schema, mensaDate *date);

/*int mensa_schema_get_path(mensaSchema *schema, int day, int food, char *path);*/

#endif
