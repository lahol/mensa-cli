/*  Libmensa - get information of available meals for a given canteen.
 *  Copyright (c) 2011, Holger Langenau
 *
 *  This file is part of Libmensa.
 *
 *  Libmensa is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Libmensa is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Libmensa.  If not, see <http://www.gnu.org/licenses/>.
 */
/** @file
 *  @ingroup schema
 *  @brief Implementation of the schema handling.
 *  This is the core of libmensa
 */
#include "mensa-schema.h"
#include "mensa-helpers.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "mensa-document.h"

#include <libxml/xmlmemory.h>
#include <libxml/parser.h>
#include <libxml/tree.h>

#include <libxml/HTMLparser.h>
#include <libxml/HTMLtree.h>

#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>

#define _XOPEN_SOURCE   /* glibc2 needs this */
#include <time.h>

/** @name Format of the source to read from. */
/* @{ */
#define MENSA_SOURCE_FORMAT_HTML     1       /**< @brief The source is an html-file */
#define MENSA_SOURCE_FORMAT_XML      2       /**< @brief The source is an xml-file */
/* @} */

/** @name Flags to control the handling of the source. */
/* @{ */
#define MENSA_SOURCE_FLAGS_STRFTIME  1<<0  /**< @brief parse source through strftime */
/* @} */

/** @brief Main source information. */
/* @{ */
struct _mensaSchemaSource {
    int id;           /**< @brief Identifier for that source, referenced by food. */
    int format;       /**< @brief Format of the source: html, xml, ... */
    char *source;     /**< @brief URI of that source */
    int flags;        /**< @brief Flags to control the handling of the source. */
};
/* @} */

/** @brief Information where to get a single food from. */
/* @{ */
struct _mensaSchemaSourceFood {
    int source_id;    /**< @brief Source identifier where to read the data from. */
    int week;         /**< @brief 0 is the current week, 1 the next and so on. */
    int day;          /**< @brief 0-6, 0 sunday */
    char *path;       /**< @brief XPath to the field of the food. */
    char *desc_path;  /**< @brief Relative path to the description of the food. */
};
/* @} */

/** @brief Description of a single food. */
/* @{ */
struct _mensaSchemaFoodDescription {
    char *identifier;  /**< @brief The identifier of the description, as found in the source. */
    char *description; /**< @brief The human readable description, the name to display */
};
/* @} */

/** @brief The schema struct.
 *  Opaque structure holding all necessary information about the schema.
 */
/* @{ */
struct _mensaSchema {
    char *schemaName;                           /**< @brief Identifier of the schema. */
    char *schemaDesc;                  /**< @brief Description of the schema. */
    struct _mensaSchemaSource *sources;         /**< @brief Sources of the schema. */
    int nsources;                               /**< @brief Number of sources of the schema. */
    struct _mensaSchemaSourceFood *foods;       /**< @brief Food descriptors of the schema. */
    int nfoods;                                 /**< @brief Number of food descriptors. */
    struct _mensaSchemaFoodDescription *fdescs; /**< @brief Food descriptions. */
    int nfdescs;                                /**< @brief Number of food descriptions. */
};
/* @} */

/** @brief Size of internal buffers.
 *  @internal
 */
#define BUFFER_SIZE    2048

/** @brief Read a schema definition from a given file.
 *
 *  A schema definition is an xml file, containing a list of sources,
 *  food descriptions and food paths.
 *
 *  @param[in] filename The filename to read the definition from.
 *  @return If successful a pointer to the new schema. Should be
 *          freed with mensa_schema_free().
 */
mensaSchema *mensa_schema_read_from_file(const char *filename)
{
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

    if (!root || !root->name || xmlStrcmp(root->name, (const xmlChar *)"mensaschema")) {
        xmlFreeDoc(doc);
        free(schema);
        fprintf(stderr, "Invalid root element.\n");
        return NULL;
    }

    for (cur = root->children; cur; cur = cur->next) {
        if (cur->type == XML_ELEMENT_NODE) {
            if (!xmlStrcmp(cur->name, (const xmlChar *)"name")) {
                content = xmlNodeGetContent(cur);
                if (content) {
                    if (!schema->schemaName) {
                        schema->schemaName = strdup((char *)content);
                    }
                    xmlFree(content);
                }
            }
            else if (!xmlStrcmp(cur->name, (const xmlChar *)"description")) {
                content = xmlNodeGetContent(cur);
                if (content) {
                    if (!schema->schemaDesc) {
                        schema->schemaDesc = strdup((char *)content);
                    }
                    xmlFree(content);
                }
            }
            else if (!xmlStrcmp(cur->name, (const xmlChar *)"source")) {
                source = malloc(sizeof(struct _mensaSchemaSource));
                assert(source);
                memset(source, 0, sizeof(struct _mensaSchemaSource));

                content = xmlGetProp(cur, (const xmlChar *)"id");
                if (content) {
                    source->id = atoi((const char *)content);
                    xmlFree(content);
                }
                else {
                    source->id = -1;
                }

                content = xmlGetProp(cur, (const xmlChar *)"format");
                if (content) {
                    if (!xmlStrcasecmp(content, (const xmlChar *)"html")) {
                        source->format = MENSA_SOURCE_FORMAT_HTML;
                    }
                    else if (!xmlStrcasecmp(content, (const xmlChar *)"xml")) {
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

                content = xmlGetProp(cur, (const xmlChar *)"flags");
                if (content) {
                    source->flags = atoi((const char *)content);
                    xmlFree(content);
                }
                else {
                    source->flags = 0;
                }

                content = xmlGetProp(cur, (const xmlChar *)"file");
                if (content) {
                    source->source = strdup((const char *)content);
                    xmlFree(content);
                }
                else {
                    source->source = NULL;
                }

                source_list = mensa_list_prepend(source_list, (void *)source);
                schema->nsources++;
                source = NULL;
            }
            else if (!xmlStrcmp(cur->name, (const xmlChar *)"fooddescription")) {
                fdesc = malloc(sizeof(struct _mensaSchemaFoodDescription));
                assert(fdesc);
                memset(fdesc, 0, sizeof(struct _mensaSchemaFoodDescription));

                content = xmlGetProp(cur, (const xmlChar *)"identifier");
                if (content) {
                    fdesc->identifier = strdup((const char *)content);
                    xmlFree(content);
                }
                else {
                    fdesc->identifier = NULL;
                }

                content = xmlNodeGetContent(cur);
                if (content) {
                    fdesc->description = strdup((const char *)content);
                    xmlFree(content);
                }
                else {
                    fdesc->description = NULL;
                }

                fdesc_list = mensa_list_prepend(fdesc_list, (void *)fdesc);
                schema->nfdescs++;
                fdesc = NULL;
            }
            else if (!xmlStrcmp(cur->name, (const xmlChar *)"food")) {
                food = malloc(sizeof(struct _mensaSchemaSourceFood));
                assert(food);
                memset(food, 0, sizeof(struct _mensaSchemaSourceFood));

                content = xmlGetProp(cur, (const xmlChar *)"source");
                if (content) {
                    food->source_id = atoi((const char *)content);
                    xmlFree(content);
                }
                else {
                    food->source_id = -2;
                }

                content = xmlGetProp(cur, (const xmlChar *)"week");
                if (content) {
                    food->week = atoi((const char *)content);
                    xmlFree(content);
                }
                else {
                    food->week = -1;
                }

                content = xmlGetProp(cur, (const xmlChar *)"day");
                if (content) {
                    food->day = atoi((const char *)content);
                    xmlFree(content);
                }
                else {
                    food->day = -1;
                }

                content = xmlGetProp(cur, (const xmlChar *)"path");
                if (content) {
                    food->path = strdup((const char *)content);
                    xmlFree(content);
                }
                else {
                    food->path = NULL;
                }

                content = xmlGetProp(cur, (const xmlChar *)"description");
                if (content) {
                    food->desc_path = strdup((const char *)content);
                    xmlFree(content);
                }
                else {
                    food->desc_path = NULL;
                }

                food_list = mensa_list_prepend(food_list, (void *)food);
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
            free((struct _mensaSchemaSource *)source_list->data);
            free(source_list);
            source_list = tmp;
        }
    }

    tmp = NULL;
    for (i = schema->nfdescs-1; i >= 0; i--) {
        if (fdesc_list) {
            memcpy(&schema->fdescs[i], fdesc_list->data, sizeof(struct _mensaSchemaFoodDescription));
            tmp = fdesc_list->next;
            free((struct _mensaSchemaFoodDescription *)fdesc_list->data);
            free(fdesc_list);
            fdesc_list = tmp;
        }
    }

    tmp = NULL;
    for (i = schema->nfoods-1; i >= 0; i--) {
        if (food_list) {
            memcpy(&schema->foods[i], food_list->data, sizeof(struct _mensaSchemaSourceFood));
            tmp = food_list->next;
            free((struct _mensaSchemaSourceFood *)food_list->data);
            free(food_list);
            food_list = tmp;
        }
    }

    xmlFreeDoc(doc);

    return schema;
}

/** @brief Free allocated memory of a schema.
 *  @param[in] schema A pointer to a schema struct to free.
 *  @see mensa_schema_read_from_file
 */
void mensa_schema_free(mensaSchema *schema)
{
    int i;
    if (schema) {
        if (schema->schemaName) {
            free(schema->schemaName);
        }
        if (schema->schemaDesc) {
            free(schema->schemaDesc);
        }
        if (schema->sources) {
            for (i = 0; i < schema->nsources; i++) {
                if (schema->sources[i].source) {
                    free(schema->sources[i].source);
                }
            }
            free(schema->sources);
        }
        if (schema->foods) {
            for (i = 0; i < schema->nfoods; i++) {
                if (schema->foods[i].path) {
                    free(schema->foods[i].path);
                }
                if (schema->foods[i].desc_path) {
                    free(schema->foods[i].desc_path);
                }
            }
            free(schema->foods);
        }
        if (schema->fdescs) {
            for (i = 0; i < schema->nfdescs; i++) {
                if (schema->fdescs[i].identifier) {
                    free(schema->fdescs[i].identifier);
                }
                if (schema->fdescs[i].description) {
                    free(schema->fdescs[i].description);
                }
            }
            free(schema->fdescs);
        }
        free(schema);
    }
}

/** @brief Return a description of the current schema.
 *
 *  This is the description of the schema as given in the schema definition.
 *  This may be empty.
 *
 *  @param[in] schema The schema to get the definition for.
 *  @param[out] desc The description of the schema if found. Should be freed
 *                   with free().
 */
void mensa_schema_get_description(mensaSchema *schema, char **desc)
{
    if (!desc) return;
    *desc = NULL;
    if (schema) {
        if (schema->schemaDesc) {
            *desc = strdup(schema->schemaDesc);
        }
    }
}

/** @brief Compound information of a schema source document.
 *  @internal
 */
/* @{ */
struct _SchemaSourceDoc {
    xmlDocPtr doc;                /**< @brief Document pointer. */
    xmlXPathContextPtr xpathCtx;  /**< @brief Context for the XPath. */
};
/* @} */

struct _SchemaSourceDoc *_mensa_schema_read_source(struct _mensaSchemaSource *source,
                                                   mensaDate *date);
void _mensa_schema_free_source_doc(struct _SchemaSourceDoc *doc);
char *_mensa_schema_get_data(struct _SchemaSourceDoc *doc, char *path);

/** @brief Return an array of available foods found for the given schema and date.
 *  @param[in] schema The schema to get the foods from.
 *  @param[in] date The date to find the data for.
 *  @return A group of meals found.
 */
MensaMealGroup *mensa_schema_get_foods(mensaSchema *schema, mensaDate *date)
{
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
    if (!date) return NULL;
    if (schema->nsources == 0 || schema->nfoods == 0) return NULL;

    docs = malloc(sizeof(struct _SchemaSourceDoc *)*schema->nsources);
    memset(docs, 0, sizeof(struct _SchemaSourceDoc *)*schema->nsources);

    for (i = 0; i < schema->nfoods; i++) {
        if ((schema->foods[i].week == date->week || schema->foods[i].week == -1) &&
                (schema->foods[i].day == date->dow || schema->foods[i].day == -1)) {
            cur_doc = NULL;
            for (j = 0; j < schema->nsources; j++) {
                if (schema->sources[j].id == schema->foods[i].source_id) {
                    if (!docs[j]) {
                        docs[j] = _mensa_schema_read_source(&schema->sources[j], date);
                    }
                    cur_doc = docs[j];
                    break;
                }
            }
            if (cur_doc) {
                data = _mensa_schema_get_data(cur_doc, schema->foods[i].path);
                if (data) {
                    list = mensa_list_prepend(list, (void *)data);
                    count++;
                    strcpy(path, schema->foods[i].path);
                    strcat(path, "/");
                    strcat(path, schema->foods[i].desc_path);
                    data = _mensa_schema_get_data(cur_doc, path);
                    dindex = -1;
                    if (data) {
                        for (k = 0; k < schema->nfdescs; k++) {
                            if (!strcmp(data, schema->fdescs[k].identifier)) {
                                dindex = k;
                                break;
                            }
                        }
                    }
                    desc_list = mensa_list_prepend(desc_list, ((void *)((long)dindex)));
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
            group->meals[i].description = (char *)list->data;
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

/** @brief Prepare source to read data from.
 *  @param[in] source The source definition.
 *  @param[in] date If the source is date dependent, i.e. if the source should
 *                  be parsed with strftime this must not be NULL. Otherwise
 *                  it is ignored.
 *  @return New compound source information.
 *  @internal
 */
struct _SchemaSourceDoc *_mensa_schema_read_source(struct _mensaSchemaSource *source,
                                                   mensaDate *date)
{
    char path_buf[2048];
    struct _SchemaSourceDoc *doc = NULL;
    struct tm tm;
    struct tm *clt;
    time_t timestamp;

    if (!source) return NULL;
    if (!source->source) return NULL;
    if (!date) return NULL;
    if (source->flags == 0) {
        strcpy(path_buf, source->source);
    }
    else {
        if (!date) return NULL;
            
        time(&timestamp);
        clt = localtime(&timestamp);
        tm.tm_mday = date->day;
        tm.tm_mon = date->month-1;
        tm.tm_year = date->year-1900;
        tm.tm_hour = clt->tm_hour;
        tm.tm_min = clt->tm_min;
        tm.tm_sec = clt->tm_sec;
        timestamp = mktime(&tm);
        memcpy(&tm, localtime(&timestamp), sizeof(struct tm));
        strftime(path_buf, 2048, source->source, &tm);
  }

  doc = malloc(sizeof(struct _SchemaSourceDoc));
  assert(doc);

  mensaDocument *srcdoc = mensa_document_get(path_buf);
  if (!srcdoc) {
      free(doc);
      return NULL;
  }

  if (source->format == MENSA_SOURCE_FORMAT_HTML) {
/*    doc->doc = htmlReadFile(path_buf, NULL, 
      HTML_PARSE_NOERROR | HTML_PARSE_NOWARNING);*/
      doc->doc = htmlReadMemory(srcdoc->data, srcdoc->size, path_buf, NULL,
              HTML_PARSE_NOERROR | HTML_PARSE_NOWARNING);
  }
  else if (source->format == MENSA_SOURCE_FORMAT_XML) {
  /*  doc->doc = xmlReadFile(path_buf, NULL, 0);*/
      doc->doc = xmlReadMemory(srcdoc->data, srcdoc->size, path_buf, NULL, 0);
  }
  else {
    fprintf(stderr, "Unknown document format.\n");
    mensa_document_free(srcdoc);
    free(doc);
    return NULL;
  }
  mensa_document_free(srcdoc);
  
  if (!doc->doc) {
    fprintf(stderr, "Error parsing document.\n");
    free(doc);
    return NULL;
  }
  
  /* get context */
  doc->xpathCtx = xmlXPathNewContext(doc->doc);
  if (!doc->xpathCtx) {
    fprintf(stderr, "Could not create context.\n");
    xmlFreeDoc(doc->doc);
    free(doc);
    return NULL;
  }
  
  return doc;
}

/** @brief Free source document information.
 *  @param[in] doc The compound information to free.
 *  @internal
 */
void _mensa_schema_free_source_doc(struct _SchemaSourceDoc *doc)
{
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

/** @brief Print all children of a node. Only for debug.
 *  @param[in] node The node to print the children.
 *  @param[in] level Used for indention.
 *  @internal
 */
void _print_children(xmlNode *node, int level)
{
    if (!node) return;
    xmlNode *cur;
    int i;
    for (cur = node->children; cur != NULL; cur = cur->next) {
        for (i = 0; i < level; i++) putc(' ', stderr);
        if (cur->type == XML_TEXT_NODE) {
            fputs((char *)cur->content, stderr);
            fputc('\n', stderr);
        }
        else {
            fputc('\"', stderr);
            fputs((char *)cur->name, stderr);
            fputc('\"', stderr);
            if (cur->nsDef) {
                fprintf(stderr, " (ns-prefix: \"%s\")", cur->nsDef->prefix);
            }
            fprintf(stderr, " (type: %d)", cur->type);
            fputc('\n', stderr);

        }
        _print_children(cur, level+1);
    }
}

/** @brief Get data from source for given path.
 *  @param[in] doc The source document.
 *  @param[in] path The XPath for the data element.
 *  @return A beatified string with the data if found, otherwise NULL.
 *          If the resulting string is empty NULL is returned.
 *  @internal
 *  @see mensa_string_beautify
 */
char *_mensa_schema_get_data(struct _SchemaSourceDoc *doc, char *path)
{
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

    xpathObj = xmlXPathEval((xmlChar *)path, doc->xpathCtx);
    if (!xpathObj) {
        fprintf(stderr, "Could not evaluate expression.\n");
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
                l = strlen((char *)content);
                if (len + l < BUFFER_SIZE) {  /* is space available? */
                    len += l;
                    strcat(buffer, (char *)content);
                }
                else {
                    len = BUFFER_SIZE;
                }
                xmlFree(content);
            }
        }
    }

    /* remove spaces and more */
    mensa_string_beautify(buffer, 1);

    xmlXPathFreeObject(xpathObj);

    if (buffer[0] != '\0') {
        return strdup(buffer);
    }
    else {
        return NULL;
    }
}
