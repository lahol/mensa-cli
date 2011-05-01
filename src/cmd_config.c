#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cmd.h"
#include "defaults.h"
#include "mensa-output.h"
#include "utils.h"

/** Check if the string is a setting of the form key=value
 *  and update this value.
 *  @param arg The string as we get it from the command line
 *  @return 0 if it was a setting, 1 otherwise
 */
int cmd_config_set(char *arg) {
  int start;
  char *buffer = NULL;
  int is_setting = 0;
  buffer = strdup(arg);
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
    }
    free(buffer);
  }
  return !is_setting;
}

int cmd_config(int argc, char **argv) {
  char *rcpath;
  int i;
  int nsettings = 0;
  int term_width;
  int len, max_len, block_width, newline_after_key, block_indent;
  DefaultsEnumResult enres;
  /* ignore argv[0] */
  for (i = 1; i < argc; i++) {
    if (cmd_config_set(argv[i]) == 0) {
      nsettings++;
    }
  }
  
  if (nsettings > 0) {
    rcpath = utils_get_rcpath();

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
