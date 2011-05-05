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
