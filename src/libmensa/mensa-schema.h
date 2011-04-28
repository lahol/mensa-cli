#ifndef __MENSASCHEMA_H__
#define __MENSASCHEMA_H__

#include "mensa-data.h"
#include "mensa-time.h"

typedef struct _mensaSchema mensaSchema;

mensaSchema * mensa_schema_read_from_file(const char *filename);
void mensa_schema_free(mensaSchema *schema);

MensaMealGroup * mensa_schema_get_foods(mensaSchema *schema, mensaDate *date);

/*int mensa_schema_get_path(mensaSchema *schema, int day, int food, char *path);*/

#endif