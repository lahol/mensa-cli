#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "utils.h"

int utils_file_exists(const unsigned char *filename) {
  struct stat f;
  return stat(filename, &f) == 0 ? 1 : 0;
}

char * utils_get_rcpath(void) {
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
