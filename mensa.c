#include "mensa.h"
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>

MensaMealGroup* mensa_get_meals(int day, int mon, int year) {
  MensaMealGroup *group = malloc(sizeof(MensaMealGroup));
  if (!group) return NULL;
  group->meals = malloc(sizeof(MensaMeal));
  group->meal_count = 1;
  if (!group->meals) {
    free(group);
    return NULL;
  }
  memset(group->meals, 0, sizeof(MensaMeal)*group->meal_count);
  group->meals[0].description = strdup("Meal description");
  
  return group;
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
