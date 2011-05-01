#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cmd.h"
#include "utils.h"
#include "defaults.h"
#include "libmensa/mensa.h"
#include "mensa-output.h"
#define _XOPEN_SOURCE
#include <time.h>

int cmd_show(int argc, char **argv) {
  mensaSchema *schema = NULL;
  int i;
  int res;
  unsigned char type_string[256];
  unsigned char *header = NULL;
  unsigned char format_header[512];
  unsigned char *date_name = NULL;
  int type_str_len = 16;
  mensaDate date;
  struct tm show_tm;
  time_t timestamp;
  int max_width;
  DefaultsError derr;
  int term_width = mensa_output_get_term_width();
  if (term_width == -1) term_width = 80;

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

  schema = mensa_schema_read_from_file("/home/lahol/Projekte/mensa/data/mensa-tuc-rh-fnor-schema.xml");
  if (!schema) {
    fprintf(stderr, "Error reading schema.\n");
    return 1;
  }
  
  if (argc >= 3) {
    res = mensa_parse_time(argv[2], &date);
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
