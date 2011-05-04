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
MensaMealGroup* mensa_get_meals(mensaSchema *schema, mensaDate *date) {
  if (!schema) return NULL;
  if (!date) return NULL;
  /*TODO: insert calculations for week and day */
  return mensa_schema_get_foods(schema, date);
}

/** @brief Free a list of meals.
 *  @param[in] meals A pointer to a mensa meal group.
 *  @see mensa_get_meals
 */
void mensa_free_meals(MensaMealGroup *meals) {
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
