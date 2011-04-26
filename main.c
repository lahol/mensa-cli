#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <ctype.h>
#include <string.h>
#include "commands.h"
#include "defaults.h"
#include "mensa.h"
#include "mensa-output.h"

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
  int type_str_len = 16;

  printf("Show\n");
  
  schema = mensa_schema_read_from_file("/home/lahol/Projekte/mensa/data/mensa-tuc-rh-schema.xml");
  if (!schema) {
    fprintf(stderr, "Error reading schema.\n");
    return 1;
  }
  
  if (argc >= 3) {
    i = 0;
    flag = 1;
    while (argv[2][i] != '\0') {
      if (!isdigit(argv[2][i++])) {
        flag = 0;
        break;
      }
    }
    if (flag) {
      day = atoi(argv[2]);
    }
  }
  
  MensaMealGroup *group = mensa_get_meals(schema, day, 0, 0);
  if (!group) {
    fprintf(stderr, "Error reading meals.\n");
    mensa_schema_free(schema);
    return 1;
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
        62, 18, 0);
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
  defaults_add("cmd.default", "show");
  defaults_add("show.date", "today");
  defaults_add("show.mensa", "rh");
  defaults_add("show.clear-cache", "no");
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

