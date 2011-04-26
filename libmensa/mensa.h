#ifndef __MENSA_H__
#define __MENSA_H__

#include "mensa-data.h"
#include "mensa-schema.h"
#include "mensa-time.h"

MensaMealGroup* mensa_get_meals(mensaSchema *schema, mensaDate *date);
void mensa_free_meals(MensaMealGroup *meals);

#endif