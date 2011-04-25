#include "mensa-schema.h"
#include "mensa-helpers.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include <libxml/xmlmemory.h>
#include <libxml/parser.h>
#include <libxml/tree.h>

#include <libxml/HTMLparser.h>
#include <libxml/HTMLtree.h>

#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>

#define MENSA_SOURCE_FORMAT_HTML     1
#define MENSA_SOURCE_FORMAT_XML      2

#define MENSA_SOURCE_FLAGS_STRFTIME  1<<0  /**< parse source through strftime */

struct _mensaSchemaSource {
  int id;           /**< identifier for that source */
  int format;       /**< html, xml, ... */
  char *source;     /**< uri of that source */
  int flags;        /**< flags */
};

struct _mensaSchemaSourceFood {
  int source_id;    /**< where to read the data from */
  int week;         /**< 0 is the current week, 1 the next and so on. */
  int day;          /**< 0-6, 0 sunday */
  char *path;       /**< xpath to the field of the food */
  char *data_path;  /**< xpath to get the data, relative to the main path */
};

struct _mensaSchemaFoodDescription {
  char *desc_path;   /**< xpath to get the description, relative to the main path */
  char *identifier;  /**< the identifier of the description */
  char *description; /**< the human readable description */
};

struct _mensaSchema {
/*  char generic_path[512];
  int *day_food; *//* number for day, number for food */
/*  int ndays;
  int nfoods;*/
  char *schemaName;                     /**< identifier of the schema */
  struct _mensaSchemaSource *sources;   /**< sources of the schema */
  int nsources;                         /**< number of sources of the schema */
  struct _mensaSchemaSourceFood *foods;       /**< food descriptors of the schema */
  int nfoods;                           /**< number of food descriptors */
  struct _mensaSchemaFoodDescription *fdescs; /**< food descriptions */
  int nfdescs;                          /**< number of food descriptions */
};

#define BUFFER_SIZE    2048

mensaSchema * mensa_schema_read_from_file(const char *filename) {
  mensaSchema *schema = malloc(sizeof(mensaSchema));
  assert(schema);
  schema->schemaName = strdup("tu-chemnitz.de-rh");

  schema->nsources = 1;
  schema->sources = malloc(sizeof(struct _mensaSchemaSource)*schema->nsources);
  schema->sources[0].id = 1;
  schema->sources[0].format = MENSA_SOURCE_FORMAT_HTML;
  schema->sources[0].source = strdup("/home/lahol/Projekte/mensa/sources/web_RH.html");
  schema->sources[0].flags = 0;
  
  schema->nfoods = 5;
  schema->foods = malloc(sizeof(struct _mensaSchemaSourceFood)*schema->nfoods);
  schema->foods[0].source_id = 1;
  schema->foods[0].week = 0;
  schema->foods[0].day = 5;
  schema->foods[0].path = strdup("/html/body/div/table[2]/tbody/tr/td/table/tbody/tr[2]/td[5]/table/tbody/");
  schema->foods[0].data_path = strdup("tr[2]/td"); /** TODO: geht für desc evtl. auch ../../tr[1]/td für xpath? */
    /* dann kann man sich das feld auch sparen*/
  schema->foods[1].source_id = 1;
  schema->foods[1].week = 0;
  schema->foods[1].day = 5;
  schema->foods[1].path = strdup("/html/body/div/table[2]/tbody/tr/td/table/tbody/tr[3]/td[5]/table/tbody/");
  schema->foods[1].data_path = strdup("tr[2]/td"); /** TODO: geht für desc evtl. auch ../../tr[1]/td für xpath? */
  schema->foods[2].source_id = 1;
  schema->foods[2].week = 0;
  schema->foods[2].day = 1;
  schema->foods[2].path = strdup("/html/body/div/table[2]/tbody/tr/td/table/tbody/tr[4]/td[1]/table/tbody/");
  schema->foods[2].data_path = strdup("tr[2]/td"); /** TODO: geht für desc evtl. auch ../../tr[1]/td für xpath? */
  schema->foods[3].source_id = 1;
  schema->foods[3].week = 0;
  schema->foods[3].day = 1;
  schema->foods[3].path = strdup("/html/body/div/table[2]/tbody/tr/td/table/tbody/tr[5]/td[1]/table/tbody/");
  schema->foods[3].data_path = strdup("tr[2]/td"); /** TODO: geht für desc evtl. auch ../../tr[1]/td für xpath? */
  schema->foods[4].source_id = 1;
  schema->foods[4].week = 0;
  schema->foods[4].day = 1;
  schema->foods[4].path = strdup("/html/body/div/table[2]/tbody/tr/td/table/tbody/tr[6]/td[1]/table/tbody/");
  schema->foods[4].data_path = strdup("tr[2]/td"); /** TODO: geht für desc evtl. auch ../../tr[1]/td für xpath? */
  
  schema->nfdescs = 6;
  schema->fdescs = malloc(sizeof(struct _mensaSchemaFoodDescription)*schema->nfdescs);
  schema->fdescs[0].desc_path = strdup("tr[1]/td");
  schema->fdescs[0].identifier = strdup("Hauptessen 1");
  schema->fdescs[0].description = strdup("Essen 1");
  schema->fdescs[1].desc_path = strdup("tr[1]/td");
  schema->fdescs[1].identifier = strdup("Hauptessen 2");
  schema->fdescs[1].description = strdup("Essen 2");
  schema->fdescs[2].desc_path = strdup("tr[1]/td");
  schema->fdescs[2].identifier = strdup("Hauptessen 3");
  schema->fdescs[2].description = strdup("Essen 3");
  schema->fdescs[3].desc_path = strdup("tr[1]/td");
  schema->fdescs[3].identifier = strdup("Hauptessen 4");
  schema->fdescs[3].description = strdup("Essen 4");
  schema->fdescs[4].desc_path = strdup("tr[1]/td");
  schema->fdescs[4].identifier = strdup("Abendessen 3");
  schema->fdescs[4].description = strdup("Abendessen 3");
  schema->fdescs[5].desc_path = strdup("tr[1]/td");
  schema->fdescs[5].identifier = strdup("Gemüsebar");
  schema->fdescs[5].description = strdup("Gemüsebar");
  
  return schema;
}

void mensa_schema_free(mensaSchema *schema) {
  if (schema) {
    free(schema);
  }
}

struct _SchemaSourceDoc {
  xmlDocPtr doc;
  xmlXPathContextPtr xpathCtx;
};

struct _SchemaSourceDoc * _mensa_schema_read_source(struct _mensaSchemaSource *source);
void _mensa_schema_free_source_doc(struct _SchemaSourceDoc *doc);
char * _mensa_schema_get_data(struct _SchemaSourceDoc *doc, char *path);

MensaMealGroup * mensa_schema_get_foods(mensaSchema *schema, int week, int day) {
  struct _SchemaSourceDoc **docs = NULL;
  struct _SchemaSourceDoc *cur_doc = NULL;
  char path[BUFFER_SIZE];
  MensaMealGroup *group = NULL;
  int count = 0;
  int i, j;
  mensaList *list = NULL;
  mensaList *tmp;
  struct _mensaSchemaSourceFood *food;
  struct _mensaSchemaSource *source;
  char *data;
  if (!schema) return NULL;
  if (schema->nsources == 0 || schema->nfoods == 0) return NULL;
  
  docs = malloc(sizeof(struct _SchemaSourceDoc*)*schema->nsources);
  memset(docs, 0, sizeof(struct _SchemaSourceDoc*)*schema->nsources);
  
  for (i = 0; i < schema->nfoods; i++) {
    if (schema->foods[i].week == week && 
        schema->foods[i].day == day) {
      cur_doc = NULL;
      for (j = 0; j < schema->nsources; j++) {
        if (schema->sources[j].id == schema->foods[i].source_id) {
          if (!docs[j]) {
            docs[j] = _mensa_schema_read_source(&schema->sources[j]);
          }
          cur_doc = docs[j];
          break;
        }
      }
      if (cur_doc) {
        strcpy(path, schema->foods[i].path);
        strcat(path, schema->foods[i].data_path);
        data = _mensa_schema_get_data(cur_doc, path);
        if (data) {
          list = mensa_list_prepend(list, (void*)data);
          count++;
        }
      }
    }
  }
  
  /* free docs */
  for (j = 0; j < schema->nsources; j++) {
    _mensa_schema_free_source_doc(docs[j]);
  }
  
  group = malloc(sizeof(MensaMealGroup));
  group->meal_count = count;
  if (count) {
    group->meals = malloc(sizeof(MensaMeal)*count);
    memset(group->meals, 0, sizeof(MensaMeal)*count);
  }
  else {
    group->meals = NULL;
  }
  
  tmp = NULL;
  for (i = count-1; i >= 0; i--) {
    if (list) {
      group->meals[i].description = (char*)list->data;
      tmp = list->next;
      free(list);
      list = tmp;
    }
  }
  
  return group;
}

struct _SchemaSourceDoc * _mensa_schema_read_source(struct _mensaSchemaSource *source) {
  if (!source) return NULL;
  if (!source->source) return NULL;
  struct _SchemaSourceDoc *doc = malloc(sizeof(struct _SchemaSourceDoc));
  assert(doc);
  if (source->format == MENSA_SOURCE_FORMAT_HTML) {
    doc->doc = htmlReadFile(source->source, NULL, 0);
  }
  else if (source->format == MENSA_SOURCE_FORMAT_HTML) {
    doc->doc = xmlReadFile(source->source, NULL, 0);
  }
  else {
    free(doc);
    return NULL;
  }
  
  if (!doc->doc) {
    free(doc);
    return NULL;
  }
  
  /* get context */
  doc->xpathCtx = xmlXPathNewContext(doc->doc);
  if (!doc->xpathCtx) {
    xmlFreeDoc(doc->doc);
    free(doc);
    return NULL;
  }
  
  return doc;
}

void _mensa_schema_free_source_doc(struct _SchemaSourceDoc *doc) {
  if (doc) {
    if (doc->xpathCtx) {
      xmlXPathFreeContext(doc->xpathCtx);
    }
    if (doc->doc) {
      xmlFreeDoc(doc->doc);
    }
    free(doc);
  }
}

char * _mensa_schema_get_data(struct _SchemaSourceDoc *doc, char *path) {
  xmlXPathObjectPtr xpathObj;
  char buffer[BUFFER_SIZE];
  xmlNodePtr cur;
  int i, size;
  xmlChar *content;
  int len = 0, l = 0;
  if (!doc || !path) {
    return NULL;
  }
  
  xpathObj = xmlXPathEvalExpression((xmlChar*)path, doc->xpathCtx);
  if (!xpathObj) {
    return NULL;
  }
  
  buffer[0] = '\0';
  
  /* get nodeset data for xpathObj->nodesetval */
  size = (xpathObj->nodesetval) ? xpathObj->nodesetval->nodeNr : 0;
  if (size == 0) {
    xmlXPathFreeObject(xpathObj);
    return NULL;
  }

  for (i = 0; i < size && len < BUFFER_SIZE-1; i++) {
    if (xpathObj->nodesetval->nodeTab[i]) {
      cur = xpathObj->nodesetval->nodeTab[i];
      content = xmlNodeGetContent(cur);
      if (content) {
        l = strlen((char*)content);
        if (len + l < BUFFER_SIZE) {  /* is space available? */
          len += l;
          strcat(buffer, (char*)content);
        }
        else {
          len = BUFFER_SIZE;
        }
        xmlFree(content);
      }
    }
  }
  
  /* ltrim, rtrim: remove leading and trailing whitespaces */
  len = strlen(buffer);
  for (i = len-1; i >= 0; i--) {
    if (buffer[i] == ' ' ||
        buffer[i] == '\t' ||
        buffer[i] == 0xd ||
        buffer[i] == 0xa) {
      buffer[i] = '\0';
    }
    else if (buffer[i] == (char)0xa0 && i > 0 && buffer[i-1] == (char)0xc2) {  /*no-break space, U+00A0 (0xc2 0xa0)*/
      /* need type conversion, because a0 < 0 for signed char (compare signed with unsigned otherwise) */
      buffer[i] = buffer[i-1] = '\0';
      i--;
    }
    else {
      break;
    }
  }
  i = 0;
  do {
    if (buffer[i] != ' ' &&
        buffer[i] != '\t' &&
        buffer[i] != 0xa &&
        buffer[i] != 0xd) {
      if (buffer[i] != (char)0xc2 || buffer[i+1] != (char)0xa0) {  /*no-break space, U+00A0 (0xc2 0xa0)*/
        break;
      }
      else {
        i++;
      }
    }
    i++;
  } while (buffer[i] != '\0');
  
  xmlXPathFreeObject(xpathObj);
  if (buffer[i] != '\0') { /* i.e. strlen == 0 */
    return strdup(&buffer[i]);
  }
  else {
    return NULL;
  }
}
