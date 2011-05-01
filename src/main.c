#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <ctype.h>
#include <string.h>
#include <locale.h>
#include <libintl.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "commands.h"
#include "defaults.h"
#include "libmensa/mensa.h"
#include "mensa-output.h"
#define _XOPEN_SOURCE
#include <time.h>

int parse_cmdline(int argc, char **argv);

int file_exists(const unsigned char *filename);

char * _get_rcpath(void);

int cmd_help(int argc, char **argv) {
  if (argc <= 2) {
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
}

int cmd_info(int argc, char **argv) {
  printf("Info\n");
  return 0;
}

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

int cmd_config(int argc, char **argv) {
  char *buffer = NULL;
  char *rcpath;
  int start;
  int i;
  int is_setting = 0;
  int nsettings = 0;
  int term_width;
  int len, max_len, block_width, newline_after_key, block_indent;
  DefaultsEnumResult enres;
  /* ignore argv[0] */
  for (i = 1; i < argc; i++) {
    buffer = strdup(argv[i]);
    is_setting = 0;
    if (buffer) {
      start = 0;
      while (buffer[start] != '\0') {
        if (buffer[start] == '=') {
          is_setting = 1;
          buffer[start] = '\0';
          start++;
          break;
        }
        start++;
      }
      if (start > 0 && is_setting) {
        defaults_update(buffer, &buffer[start]);
        nsettings++;
      }
      free(buffer);
    }
  }
  
  if (nsettings > 0) {
    rcpath = _get_rcpath();

    if (defaults_write(rcpath) != 0) {
      fprintf(stderr, "Error writing config.\n");
    }
    else {
      fprintf(stdout, "Written configuration to `%s'.\n", rcpath);
    }
    if (rcpath) {
      free(rcpath);
    }
  }
  else { /* no setting specified */
    term_width = mensa_output_get_term_width();
    if (term_width == -1) term_width = 80;
    defaults_enum(NULL, &enres);
    if (enres.numResults > 0) {
      max_len = 1;
      newline_after_key = 0;
      /* get max length of key */
      for (i = 0; i < enres.numResults; i++) {
        if (enres.keys[i]) {
          len = strlen(enres.keys[i]);
          if (len > max_len) max_len = len;
        }
      }
      
      block_width = term_width - max_len - 2;
      if (block_width < 20) {
        newline_after_key = 1;
        block_indent = 4;
        block_width = term_width-block_indent;
      }
      else {
        block_indent = max_len;
      }
      
      /* output all settings */
      for (i = 0; i < enres.numResults; i++) {
        if (enres.keys[i]) {
          mensa_output_fixed_len_str(stdout, enres.keys[i], max_len);
          if (newline_after_key) {
            fputc('\n', stdout);
          }
          else {
            fputc(' ', stdout);
            fputc(' ', stdout);
          }
          if (enres.values[i] && enres.values[i][0] != '\0') {
            /* do we have something to display */
            mensa_output_block(stdout, enres.values[i], 
                               block_width, block_indent,
                               newline_after_key);  
          }
          else {
            /* newline if no value was given */
            fputc('\n', stdout);
          }
        }
      }
      defaults_enum_result_free(&enres);
    }
  }
  return 0;
}

int main(int argc, char **argv) {
/*  parse_cmdline(argc, argv);*/
  char *rcpath = NULL;
  setlocale(LC_ALL, "");
  
  defaults_add("cmd.default", "show");
  defaults_add("show.date", "today");
  defaults_add("show.schema", "");
  defaults_add("show.clear-cache", "no");
  defaults_add("show.header", "Offers for %A, %x");
  defaults_add("show.max-width", "80");
  
  rcpath = _get_rcpath();
  
  if (file_exists(rcpath)) {
    if (defaults_read(rcpath) != 0) {
      defaults_free();
      if (rcpath) {
        free(rcpath);
      }
      return 1;
    }
  }

  command_add("help", cmd_help, NULL);
  command_add("info", cmd_info, NULL);
  command_add("show", cmd_show, NULL);
  command_add("config", cmd_config, NULL);
  
  command_call(argc, argv);
  
  if (rcpath) {
    free(rcpath);
  }

  defaults_free();
  
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

int file_exists(const unsigned char *filename) {
  struct stat f;
  return stat(filename, &f) == 0 ? 1 : 0;
}

char * _get_rcpath(void) {
  char *home;
  char *rcpath;
  int home_len;
  home = getenv("HOME");
  if (home) {
    home_len = strlen(home);
  }
  else {
    home_len = 0;
  }
  rcpath = malloc(sizeof(char)*(home_len+10));
  if (home) {
    strcpy(rcpath, home);
  }
  else {
    rcpath[0] = '\0';
  }
  
  /* ensure that last token is no /, we add that later */
  if (home_len > 0) {
    if (rcpath[home_len-1] == '/') {
      rcpath[home_len-1] = '\0';
    }
  }
  
  strcat(rcpath, "/");
  strcat(rcpath, ".mensarc");
  
  return rcpath;
}
