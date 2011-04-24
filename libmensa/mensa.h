#ifndef __MENSA_H__
#define __MENSA_H__

#include "mensaschema.h"

typedef struct _MensaMeal {
  char *description;
} MensaMeal;

typedef struct _MensaMealGroup {
  MensaMeal *meals;
  int meal_count;
} MensaMealGroup;

MensaMealGroup* mensa_get_meals(int day, int mon, int year);
void mensa_free_meals(MensaMealGroup *meals);

#endif