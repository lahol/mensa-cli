#ifndef __MENSA_H__
#define __MENSA_H__

#include "mensa-data.h"
#include "mensa-schema.h"

MensaMealGroup* mensa_get_meals(mensaSchema *schema, int day, int mon, int year);
void mensa_free_meals(MensaMealGroup *meals);

#endif