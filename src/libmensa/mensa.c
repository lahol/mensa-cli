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
 *  @ingroup libmensa
 *  Implementation of the main interface for libmensa.
 */
#include "mensa.h"
#include "mensa-schema.h"
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>

/** @brief Return a list of the available meals.
 *  Return a list of the available meals for the given schema and date.
 *  @param[in] schema Pointer to a schema object as returned by
                      mensa_schema_read_from_file().
 *  @param[in] date Pointer to a mensaDate-struct. Should be translated
 *                  via mensa_translate_date().
 *  @return A pointer to a mensa meal group. Should be freed via mensa_free_meals().
 */
MensaMealGroup *mensa_get_meals(mensaSchema *schema, mensaDate *date)
{
    if (!schema) return NULL;
    if (!date) return NULL;
    /*TODO: insert calculations for week and day */
    return mensa_schema_get_foods(schema, date);
}

/** @brief Free a list of meals.
 *  @param[in] meals A pointer to a mensa meal group.
 *  @see mensa_get_meals
 */
void mensa_free_meals(MensaMealGroup *meals)
{
    int i;
    if (meals) {
        for (i = 0; i < meals->meal_count; i++) {
            if (meals->meals[i].description) {
                free(meals->meals[i].description);
            }
        }
        free(meals->meals);
    }
    free(meals);
}
