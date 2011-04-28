#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <ctype.h>
#include <string.h>
#include <locale.h>
#include <libintl.h>
#include "commands.h"
#include "defaults.h"
#include "libmensa/mensa.h"
#include "mensa-output.h"
#define _XOPEN_SOURCE
#include <time.h>

int parse_cmdline(int argc, char **argv);

int cmd_help(int argc, char **argv) {
  if (argc == 2) {
    printf("Usage: %s [cmd [args]]\n", argv[0]);
    printf(
"\nCommands: \n"
"    help            Show this help. \n"
"    info            Show information about this program. \n"
"    show            Show meals. \n"
"    config          Show configuration or set values. \n"
"\n"    
"Use %s help cmd to show help about this command.\n", argv[0]);
    return 0;
  }
  else if (argc > 2) {
    return command_help(argv[1]);
  } 
  else { /* Should never reach this point. */
    return 1;
  }
}

int cmd_info(int argc, char **argv) {
  printf("Info\n");
  return 0;
}

int cmd_show(int argc, char **argv) {
  mensaSchema *schema = NULL;
  int i;
  int day = 1;
  int flag;
  char type_string[256];
  char header[256];
  char format_header[512];
  int type_str_len = 16;
  mensaDate date;
  struct tm show_tm;
  time_t timestamp;
  int term_width = mensa_output_get_term_width();
  if (term_width == -1) term_width = 80;
  /*i = 2;*/
   /*try every command for time */
/*  while (!arg_time && i < argc) {
    arg_time = cmd_parse_time(argv[i]);
  }*/

  defaults_get("show.header", header);
  
  schema = mensa_schema_read_from_file("/home/lahol/Projekte/mensa/data/mensa-tuc-rh-fnor-schema.xml");
  if (!schema) {
    fprintf(stderr, "Error reading schema.\n");
    return 1;
  }
  
  if (argc >= 3) {
    mensa_parse_time(argv[2], &date);
  }
  else {
    mensa_parse_time(NULL, &date);
  }
  mensa_translate_date(&date);
  
  MensaMealGroup *group = mensa_get_meals(schema, &date);
  if (!group) {
    fprintf(stderr, "Error reading meals.\n");
    mensa_schema_free(schema);
    return 1;
  }
  
  if (header[0] != '\0') {
    show_tm.tm_mday = date.day;
    show_tm.tm_mon = date.month-1;
    show_tm.tm_year = date.year-1900;
    show_tm.tm_hour = show_tm.tm_min = show_tm.tm_sec = 0;
    timestamp = mktime(&show_tm);
    memcpy(&show_tm, localtime(&timestamp), sizeof(struct tm));
    if (strftime(format_header, 512, header, &show_tm)) {
      mensa_output_block(stdout, format_header, term_width, 0, 1);
    }
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

int cmd_config(int argc, char **argv) {
  printf("Config\n");
  return 0;
}

int main(int argc, char **argv) {
/*  parse_cmdline(argc, argv);*/
  setlocale(LC_ALL, "");

  defaults_add("cmd.default", "show");
  defaults_add("show.date", "today");
  defaults_add("show.mensa", "rh");
  defaults_add("show.clear-cache", "no");
  defaults_add("show.header", "Offers for %A, %x");
  command_add("help", cmd_help, NULL);
  command_add("info", cmd_info, NULL);
  command_add("show", cmd_show, NULL);
  command_add("config", cmd_config, NULL);
  
  command_call(argc, argv);
  
  return 0;
}

int parse_cmdline(int argc, char **argv) {
  int c;
  static int clear_cache_flag;
  int option_index = 0;
  static struct option long_options[] = {
    { "clear-cache", no_argument, &clear_cache_flag, 1 },
    { "date", optional_argument, 0, 'd' },
    { 0, 0, 0, 0}
  };
  
  while ((c = getopt_long(argc, argv, "d:c", 
                          long_options, &option_index)) != -1) {
    switch (c) {
      case 0:
        printf("option 0\n");
        break;
      case 'c':
        printf("option c\n");
        break;
      case 'd':
        printf("option d\n");
        break;
      case '?':
        printf("unknown\n");
        break;
      default:
        printf("default\n");
    }
  }
  if (optind < argc) {
    printf("non-option ARGV-elements: ");
    while (optind < argc) {
      printf("%s ", argv[optind++]);
    }
    putchar('\n');
  }
  return 0;
}

