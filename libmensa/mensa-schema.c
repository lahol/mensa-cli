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
  char *desc_path;  /**< relative path to the description */
  /*char *data_path;*/  /**< xpath to get the data, relative to the main path */
};

struct _mensaSchemaFoodDescription {
/*  char *desc_path;*/   /**< xpath to get the description, relative to the main path */
  char *identifier;  /**< the identifier of the description */
  char *description; /**< the human readable description */
};

struct _mensaSchema {
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
  mensaSchema *schema = NULL;
  xmlDocPtr doc = NULL;
  xmlNodePtr cur;
  xmlNodePtr root;
  xmlChar *content;
  mensaList *source_list = NULL;
  mensaList *fdesc_list = NULL;
  mensaList *food_list = NULL;
  mensaList *tmp = NULL;
  struct _mensaSchemaSource *source = NULL;
  struct _mensaSchemaSourceFood *food = NULL;
  struct _mensaSchemaFoodDescription *fdesc = NULL;
  
  int i; 
  
  if (!filename) {
    fprintf(stderr, "Invalid filename.\n");
    return NULL;
  }
  
  doc = xmlParseFile(filename);
  if (!doc) {
    fprintf(stderr, "Could not parse file.\n");
    return NULL;
  }
  
  schema = malloc(sizeof(mensaSchema));
  assert(schema);
  memset(schema, 0, sizeof(mensaSchema));
  
  root = xmlDocGetRootElement(doc);
  
  if (!root || !root->name || !xmlStrcmp(root->name, (const xmlChar*)"mensaprofile")) {
    xmlFreeDoc(doc);
    free(schema);
    fprintf(stderr, "Invalid root element.\n");
    return NULL;
  }
  
  for (cur = root->children; cur; cur = cur->next) {
    if (cur->type == XML_ELEMENT_NODE) {
      if (!xmlStrcmp(cur->name, (const xmlChar*)"name")) {
        content = xmlNodeGetContent(cur);
        if (content) {
          if (!schema->schemaName) {
            schema->schemaName = strdup((char*)content);
            xmlFree(content);
          }
        }
      }
      else if (!xmlStrcmp(cur->name, (const xmlChar*)"source")) {
        source = malloc(sizeof(struct _mensaSchemaSource));
        assert(source);
        memset(source, 0, sizeof(struct _mensaSchemaSource));
        
        content = xmlGetProp(cur, (const xmlChar*)"id");
        if (content) {
          source->id = atoi((const char*)content);
          xmlFree(content);
        }
        else {
          source->id = -1;
        }
        
        content = xmlGetProp(cur, (const xmlChar*)"format");
        if (content) {
          if (!xmlStrcasecmp(content, "html")) {
            source->format = MENSA_SOURCE_FORMAT_HTML;
          }
          else if (!xmlStrcasecmp(content, "xml")) {
            source->format = MENSA_SOURCE_FORMAT_XML;
          }
          else {
            source->format = 0;
          }
          xmlFree(content);
        }
        else {
          source->format = 0;
        }
        
        content = xmlGetProp(cur, (const xmlChar*)"flags");
        if (content) {
          source->flags = atoi((const char*)content);
          xmlFree(content);
        }
        else {
          source->flags = 0;
        }
        
        content = xmlGetProp(cur, (const xmlChar*)"file");
        if (content) {
          source->source = strdup((const char*)content);
          xmlFree(content);
        }
        else {
          source->source = NULL;
        }
        
        source_list = mensa_list_prepend(source_list, (void*)source);
        schema->nsources++;
        source = NULL;
      }
      else if (!xmlStrcmp(cur->name, (const xmlChar*)"fooddescription")) {
        fdesc = malloc(sizeof(struct _mensaSchemaFoodDescription));
        assert(fdesc);
        memset(fdesc, 0, sizeof(struct _mensaSchemaFoodDescription));
        
        content = xmlGetProp(cur, (const xmlChar*)"identifier");
        if (content) {
          fdesc->identifier = strdup((const char*)content);
          xmlFree(content);
        }
        else {
          fdesc->identifier = NULL;
        }
        
        content = xmlNodeGetContent(cur);
        if (content) {
          fdesc->description = strdup((const char*)content);
          xmlFree(content);
        }
        else {
          fdesc->description = NULL;
        }
        
        fdesc_list = mensa_list_prepend(fdesc_list, (void*)fdesc);
        schema->nfdescs++;
        fdesc = NULL;
      }
      else if (!xmlStrcmp(cur->name, (const xmlChar*)"food")) {
        food = malloc(sizeof(struct _mensaSchemaSourceFood));
        assert(food);
        memset(food, 0, sizeof(struct _mensaSchemaSourceFood));
        
        content = xmlGetProp(cur, (const xmlChar*)"source");
        if (content) {
          food->source_id = atoi((const char*)content);
          xmlFree(content);
        }
        else {
          food->source_id = -2;
        }
        
        content = xmlGetProp(cur, (const xmlChar*)"week");
        if (content) {
          food->week = atoi((const char*)content);
          xmlFree(content);
        }
        else {
          food->week = -1;
        }
        
        content = xmlGetProp(cur, (const xmlChar*)"day");
        if (content) {
          food->day = atoi((const char*)content);
          xmlFree(content);
        }
        else {
          food->day = -1;
        }
        
        content = xmlGetProp(cur, (const xmlChar*)"path");
        if (content) {
          food->path = strdup((const char*)content);
          xmlFree(content);
        }
        else {
          food->path = NULL;
        }
        
        content = xmlGetProp(cur, (const xmlChar*)"description");
        if (content) {
          food->desc_path = strdup((const char*)content);
          xmlFree(content);
        }
        else {
          food->desc_path = NULL;
        }
        
        food_list = mensa_list_prepend(food_list, (void*)food);
        schema->nfoods++;
        food = NULL;
      }
    }
  }
  
  schema->sources = malloc(sizeof(struct _mensaSchemaSource)*schema->nsources);
  assert(schema->sources);
  schema->foods   = malloc(sizeof(struct _mensaSchemaSourceFood)*schema->nfoods);
  assert(schema->foods);
  schema->fdescs  = malloc(sizeof(struct _mensaSchemaFoodDescription)*schema->nfdescs);
  assert(schema->fdescs);
  
  tmp = NULL;
  for (i = schema->nsources-1; i >= 0; i--) {
    if (source_list) {
      memcpy(&schema->sources[i], source_list->data, sizeof(struct _mensaSchemaSource));
      tmp = source_list->next;
      free((struct _mensaSchemaSource*)source_list->data);
      free(source_list);
      source_list = tmp;
    }
  }
  
  tmp = NULL;
  for (i = schema->nfdescs-1; i >= 0; i--) {
    if (fdesc_list) {
      memcpy(&schema->fdescs[i], fdesc_list->data, sizeof(struct _mensaSchemaFoodDescription));
      tmp = fdesc_list->next;
      free((struct _mensaSchemaFoodDescription*)fdesc_list->data);
      free(fdesc_list);
      fdesc_list = tmp;
    }
  }
  
  tmp = NULL;
  for (i = schema->nfoods-1; i >= 0; i--) {
    if (food_list) {
      memcpy(&schema->foods[i], food_list->data, sizeof(struct _mensaSchemaSourceFood));
      tmp = food_list->next;
      free((struct _mensaSchemaSourceFood*)food_list->data);
      free(food_list);
      food_list = tmp;
    }
  }
 
  xmlFreeDoc(doc);
  
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
  int i, j, k;
  int dindex;
  mensaList *list = NULL;
  mensaList *desc_list = NULL;
  mensaList *tmp, *tmpdesc;
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
        /*strcpy(path, schema->foods[i].path);
        strcat(path, schema->foods[i].data_path);*/
        data = _mensa_schema_get_data(cur_doc, schema->foods[i].path);
        if (data) {
          list = mensa_list_prepend(list, (void*)data);
          count++;
          strcpy(path, schema->foods[i].path);
          strcat(path, "/");
          strcat(path, schema->foods[i].desc_path);
/*          fprintf(stderr, "desc-path: %s\n", path);*/
          data = _mensa_schema_get_data(cur_doc, path);
          dindex = -1;
          if (data) {
/*            fprintf(stderr, "try to find \"%s\" in %d descs\n", data, schema->nfdescs);*/
            for (k = 0; k < schema->nfdescs; k++) {
/*              fprintf(stderr, "  %d: %s\n", k, schema->fdescs[k].identifier);*/
              if (!strcmp(data, schema->fdescs[k].identifier)) {
                dindex = k;
                break;
              }
            }
          }
          desc_list = mensa_list_prepend(desc_list, ((void*)((long)dindex)));
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
  tmpdesc= NULL;
  for (i = count-1; i >= 0; i--) {
    if (list) {
      group->meals[i].description = (char*)list->data;
      tmp = list->next;
      free(list);
      list = tmp;
      if (desc_list) {
        if (((int)((long)desc_list->data)) >= 0 && 
            ((int)((long)desc_list->data)) < schema->nfdescs) {
          group->meals[i].type = 
            strdup(schema->fdescs[((int)((long)desc_list->data))].description);
        }
        else {
          group->meals[i].type = NULL;
        }
        tmpdesc = desc_list->next;
        free(desc_list);
        desc_list = tmpdesc;
      }
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
    fprintf(stderr, "!doc || !path");
    return NULL;
  }
  
  xpathObj = xmlXPathEvalExpression((xmlChar*)path, doc->xpathCtx);
  if (!xpathObj) {
    fprintf(stderr, "could not evaluate expression\n");
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
/*  len = strlen(buffer);
  for (i = len-1; i >= 0; i--) {
    if (buffer[i] == ' ' ||
        buffer[i] == '\t' ||
        buffer[i] == 0xd ||
        buffer[i] == 0xa) {
      buffer[i] = '\0';
    }
    else if (buffer[i] == (char)0xa0 && i > 0 && buffer[i-1] == (char)0xc2) {  *//*no-break space, U+00A0 (0xc2 0xa0)*/
      /* need type conversion, because a0 < 0 for signed char (compare signed with unsigned otherwise) *//*
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
      if (buffer[i] != (char)0xc2 || buffer[i+1] != (char)0xa0) {  *//*no-break space, U+00A0 (0xc2 0xa0)*/
/*        break;
      }
      else {
        i++;
      }
    }
    i++;
  } while (buffer[i] != '\0');*/
  mensa_string_beautify(buffer, 1);
  
  xmlXPathFreeObject(xpathObj);
  
  if (buffer[0] != '\0') {
    return strdup(buffer);
  }
  else {
    return NULL;
  }
  
/*  if (buffer[i] != '\0') {*/ /* i.e. strlen == 0 *//*
    return strdup(&buffer[i]);
  }
  else {
    return NULL;
  }*/
}

