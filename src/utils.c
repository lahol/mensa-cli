/** @file
 *  @ingroup utils
 *  Implementation of utility functions.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>

#include "utils.h"

/** Check whether a given file exists.
 *  @param[in] filename The filename to check.
 *  @return 1 if the file exists 0 otherwise.
 */
int utils_file_exists(const unsigned char *filename) {
  struct stat f;
  return stat(filename, &f) == 0 ? 1 : 0;
}

/** Get the absolute path of the rc-file.
 *  @return A pointer to a newly allocated string containing
 *          the path to the rc-file.
 */
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

/** Insert data at the beginning of the given list. Each list element
 *  should be freed with free().
 *  @param[in] list A pointer to the list where the data should be
 *                  stored or NULL if the list is empty.
 *  @param[in] data The data to store.
 *  @return The new beginning of the list.
 */
UtilsList *utils_list_prepend(UtilsList *list, void *data) {
  UtilsList *ins = malloc(sizeof(UtilsList));
  assert(ins);
  ins->next = list;
  ins->data = data;
  return ins;
}

