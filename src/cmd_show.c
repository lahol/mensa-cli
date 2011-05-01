#define _XOPEN_SOURCE
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "cmd.h"
#include "utils.h"
#include "defaults.h"
#include "libmensa/mensa.h"
#include "mensa-output.h"

int _cmd_show_parse_cmdline(int argc, char **argv, 
                            char **date, 
                            char **schema);

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
        return 1;
      }
    }
    else {
      fprintf(stderr, "No schema path specified.\n");
      return 1;
    }
  }
  else {
    fprintf(stderr, "No schema specified.\n");
    return 1;
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
  
  MensaMealGroup *group = mensa_get_meals(schema, &date);
  if (!group) {
    fprintf(stderr, "Error reading meals.\n");
    mensa_schema_free(schema);
    return 1;
  }
  
  defaults_get("show.header", &header);
  if (header) {
    show_tm.tm_mday = date.day;
    show_tm.tm_mon = date.month-1;
    show_tm.tm_year = date.year-1900;
    show_tm.tm_hour = show_tm.tm_min = show_tm.tm_sec = 0;
    timestamp = mktime(&show_tm);
    memcpy(&show_tm, localtime(&timestamp), sizeof(struct tm));
    if (strftime(format_header, 512, header, &show_tm)) {
      mensa_output_block(stdout, format_header, term_width, 0, 1);
    }
    free(header);
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
 *  are a valid date, valid schema or settings; ignore argv[0]
 */
int _cmd_show_parse_cmdline(int argc, char **argv, 
                            char **date, 
                            char **schema) {
  int i, j;
  DefaultsEnumResult schemata;
  char *remainder;
  struct tm arg_time;
  int nused = 1;
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
            /* check part after `schema.' */
            if (!strcasecmp(&schemata.keys[j][7], argv[i])) {
              used[i] = 1;
              nused++;
              if (schema) *schema = argv[i];
              break;
            }
            else if (!strcasecmp(schemata.keys[j], argv[i])) {
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
                            