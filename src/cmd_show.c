/*  Mensa-CLI - a command line interface for libmensa
 *  Copyright (C) 2011, Holger Langenau
 *
 *  This file is part of Mensa-CLI.
 *
 *  Mensa-CLI is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 * 
 *  Mensa-CLI is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Mensa-CLI.  If not, see <http://www.gnu.org/licenses/>.
 */
/** @file
 *  @ingroup commands
 *  Implementation of the show command.
 */
#define _XOPEN_SOURCE
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <strings.h>

#include "cmd.h"
#include "utils.h"
#include "defaults.h"
#include "libmensa/mensa.h"
#include "mensa-output.h"

int _cmd_show_parse_cmdline(int argc, char **argv, 
                            char **date, 
                            char **schema);

/** Run the show command
 *  @param[in] argc The number of command line arguments.
 *  @param[in] argv The command line arguments.
 *  @return 0 on success.
 */
int cmd_show(int argc, char **argv) {
  mensaSchema *schema = NULL;
  int i;
  int res;
  unsigned char type_string[256];
  unsigned char *header = NULL;
  unsigned char format_header[512];
  unsigned char *date_name = NULL;
  char *arg_date = NULL;
  char *arg_schema = NULL; 
  unsigned char *schema_path = NULL;
  char *schema_name = NULL;
  unsigned char *default_schema = NULL;
  unsigned char *schema_desc = NULL;
  unsigned char *schema_desc_key = NULL;
  int show_desc;
  int type_str_len = 16;
  mensaDate date;
  struct tm show_tm;
  time_t timestamp;
  int max_width;
  DefaultsError derr;
  int term_width = mensa_output_get_term_width();
  if (term_width == -1) term_width = 80;
  
  /* read command line */
  if (_cmd_show_parse_cmdline(argc, argv, &arg_date, &arg_schema) != 0) {
    return 1;
  }

  /* if we have a really wide terminal use specified block width */  
  derr = defaults_get_int("show.max-width", &max_width);
  if (derr != DEFAULTS_ERROR_OK) {
    fprintf(stderr, "Config mismatch: show.max-width.\n");
  }
  else {
    if (max_width > 0 && max_width < term_width) {
      term_width = max_width;
    }
  }
  
  if (defaults_get_boolean("show.description", &show_desc) != DEFAULTS_ERROR_OK) {
    show_desc = 1;
  }
  
  if (arg_date) {
    res = mensa_parse_time(arg_date, &date);
  }
  else {
    defaults_get("show.date", &date_name);
    res = mensa_parse_time(date_name, &date);
    if (date_name) {
      free(date_name);
    }
  }
  
  if (res) {
    fprintf(stderr, "Could not parse date.\n");
    return 1;
  }

  mensa_translate_date(&date);

  if (arg_schema) {
    if (arg_schema[0] != '\0') {
      schema_name = malloc(sizeof(unsigned char)*(strlen(arg_schema)+8));
      assert(schema_name);
      strcpy(schema_name, "schema.");
      strcat(schema_name, arg_schema);
    }
  }
  else {
    defaults_get("show.schema", &default_schema);
    if (default_schema) {
      if (default_schema[0] != '\0') {
        schema_name = malloc(sizeof(unsigned char)*(strlen(default_schema)+8));
        assert(schema_name);
        strcpy(schema_name, "schema.");
        strcat(schema_name, default_schema);
      }
      free(default_schema);
    }
  }
  
  if (schema_name) {
    defaults_get(schema_name, &schema_path);
    if (schema_path) {
      schema = mensa_schema_read_from_file(schema_path);
      free(schema_path);
      if (!schema) {
        fprintf(stderr, "Error reading schema.\n");
        free(schema_name);
        return 1;
      }
    }
    else {
      fprintf(stderr, "No schema path specified.\n");
      mensa_output_block(stderr, "Try running mensa-init.sh in the scripts "
                      "directory that ships with the source to "
                      "generate a basic configuration file.",
                      term_width, 0, 1);
      free(schema_name);
      return 1;
    }
  }
  else {
    fprintf(stderr, "No schema specified.\n");
    mensa_output_block(stderr, "Try running mensa-init.sh in the scripts "
                    "directory that ships with the source to "
                    "generate a basic configuration file or set "
                    "a default schema with\nshow.schema=<schema_name>",
                    term_width, 0, 1);
    return 1;
  }
  
  if (show_desc) {
    schema_desc_key = malloc(sizeof(unsigned char)*(strlen(schema_name)+13));
    strcpy(schema_desc_key, schema_name);
    strcat(schema_desc_key, ".description");
    derr = defaults_get(schema_desc_key, &schema_desc);
    if (derr != DEFAULTS_ERROR_OK) {
      mensa_schema_get_description(schema, &schema_desc);
    }
    free(schema_desc_key);
  }
  free(schema_name);
    
  
  MensaMealGroup *group = mensa_get_meals(schema, &date);
  if (!group) {
    fprintf(stderr, "Error reading meals.\n");
    mensa_schema_free(schema);
    return 1;
  }
  
  defaults_get("show.header", &header);
  if (header && header[0] != '\0') {
    show_tm.tm_mday = date.day;
    show_tm.tm_mon = date.month-1;
    show_tm.tm_year = date.year-1900;
    show_tm.tm_hour = show_tm.tm_min = show_tm.tm_sec = 0;
    show_tm.tm_isdst = -1;
    timestamp = mktime(&show_tm);
    memcpy(&show_tm, localtime(&timestamp), sizeof(struct tm));
    if (strftime(format_header, 512, header, &show_tm)) {
      mensa_output_block(stdout, format_header, term_width, 0, 1);
    }
    free(header);
  }
  
  if (show_desc && schema_desc && schema_desc[0] != 0) {
    mensa_output_block(stdout, schema_desc, term_width, 0, 1);
    free(schema_desc);
  }
  
  if (group->meal_count > 0) {
    for (i = 0; i < group->meal_count; i++) {
      if (group->meals[i].type) {
        strcpy(type_string, group->meals[i].type);
      }
      else {
        sprintf(type_string, "Meal %d", i+1);
      }
      mensa_output_fixed_len_str(stdout, type_string, type_str_len);
      printf(": ");
      mensa_output_block(stdout, group->meals[i].description,
        term_width-18, 18, 0);
    }
  }
  else {
    printf("No data available.\n");
  }
  
  mensa_free_meals(group);
  mensa_schema_free(schema);
  return 0;
}

/** Loop through command line arguments, check whether they
 *  are a valid date, valid schema or settings; ignore argv[0].
 *  Return an error if not all arguments could be recognized.
 *  @param[in] argc The number of command line arguments.
 *  @param[in] argv The command line arguments.
 *  @param[out] date The command line argument containing a date
 *                   or NULL if none exists.
 *  @param[out] schema The name of the schema if specified.
 *  @return 0 if all arguments were recognized, 1 if an error occured.
 *  @internal
 */
int _cmd_show_parse_cmdline(int argc, char **argv, 
                            char **date, 
                            char **schema) {
  int i, j, k;
  DefaultsEnumResult schemata;
  char *remainder;
  struct tm arg_time;
  int nused = 1;
  int ndots;
  int *used = malloc(sizeof(int)*argc);
  assert(used);
  memset(used, 0, sizeof(int)*argc);
  used[0] = 1;
  /* reset them */
  if (date) *date = NULL;
  if (schema) *schema = NULL;
  /* check settings first (possible new schema) */
  for (i = 1; i < argc; i++) {
    if (cmd_config_set(argv[i]) == 0) {
      used[i] = 1;
      nused++;
    }
  }
  
  /** read possible schemata */
  defaults_enum("schema.", &schemata);
  
  /* check unused (settings) for date and time */
  for (i = 1; i < argc; i++) {
    if (argv[i] && !used[i]) {
      if (!strcmp(argv[i], "show")) {
        /* command name, do nothing */
        used[i] = 1;
        nused++;
      }
      else {
        remainder = strptime(argv[i], "%Y-%m-%d", &arg_time);
        if ((remainder && remainder[0] == '\0') || 
            (!remainder && 
              (!strcasecmp(argv[i], "today") ||
               !strcasecmp(argv[i], "tomorrow") ||
               !strcasecmp(argv[i], "monday") ||
               !strcasecmp(argv[i], "tuesday") ||
               !strcasecmp(argv[i], "wednesday") ||
               !strcasecmp(argv[i], "thursday") ||
               !strcasecmp(argv[i], "friday") ||
               !strcasecmp(argv[i], "saturday") ||
               !strcasecmp(argv[i], "sunday")))) {
          /* is a valid date */
          used[i] = 1;
          nused++;
          if (date) *date = argv[i];
        }
        else {
          for (j = 0; j < schemata.numResults; j++) {
            /* first check if this is a valid schema name, i.e. contains max 1 . */
            ndots = 0;
            for (k = 0; argv[i][k] != '\0'; k++) {
              if (argv[i][k] == '.') {
                ndots++;
              }
            }
            /* check part after `schema.' */
            if (ndots == 0 && !strcasecmp(&schemata.keys[j][7], argv[i])) {
              used[i] = 1;
              nused++;
              if (schema) *schema = argv[i];
              break;
            }
            else if (ndots == 1 && !strcasecmp(schemata.keys[j], argv[i])) {
              /* full name specified */
              used[i] = 1;
              nused++;
              /* but caller wants only part after . */
              if (schema) *schema = &argv[i][7];
              break;
            }
          }
        }
      }
    }
  }
  
  defaults_enum_result_free(&schemata);
  
  /* check if all arguments could be used */
  if (nused < argc) {
    for (i = 1; i < argc; i++) {
      if (!used[i]) {
        fprintf(stderr, "Unrecognized argument: %s\n", argv[i]);
      }
    }
    free(used);
    return 1;
  }

  free(used);
  return 0;
}
                            
