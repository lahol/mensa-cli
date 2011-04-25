#ifndef __MENSA_DATA_H__
#define __MENSA_DATA_H__

typedef struct _MensaMeal {
  char *description;
} MensaMeal;

typedef struct _MensaMealGroup {
  MensaMeal *meals;
  int meal_count;
} MensaMealGroup;

#endif