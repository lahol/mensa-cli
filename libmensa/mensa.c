#include "mensa.h"
#include "mensa-schema.h"
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>

MensaMealGroup* mensa_get_meals(mensaSchema *schema, int day, int mon, int year) {
  if (!schema) return NULL;
  /*TODO: insert calculations for week and day */
  return mensa_schema_get_foods(schema, 0, 5);
}

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
