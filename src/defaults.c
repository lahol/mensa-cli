#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>
#include <assert.h>

#include "defaults.h"

#define DEFAULTS_LIST_FLAG_NONE          0     /**< no flags */
#define DEFAULTS_LIST_FLAG_MODIFIED      1<<0  /**< was modified by user (command) */
#define DEFAULTS_LIST_FLAG_WRITTEN       1<<1  /**< has been written to rc file */

typedef struct _DefaultsList DefaultsList;
struct _DefaultsList {
  char *key;
  char *value;
  unsigned long flags;
  DefaultsList *next;
};

DefaultsList *_defaults_list = NULL;

DefaultsList * _defaults_set(char *key, char *value);
DefaultsList * _defaults_get_key(char *key);

int defaults_read(char *filename) {
  /* if filename is NULL use home-folder*/
  return 0;
}

int defaults_write(char *filename) {
  return 0;
}

void defaults_free(void) {
  DefaultsList *tmp;
  while (_defaults_list) {
    tmp = _defaults_list->next;
    if (_defaults_list->key) {
      free(_defaults_list->key);
    }
    if (_defaults_list->value) {
      free(_defaults_list->value);
    }
    free(_defaults_list);
    _defaults_list = tmp;
  }
}

void defaults_get(char *key, char *value, int len) {
  DefaultsList *tmp = _defaults_get_key(key);
  if (value) {
    if (!tmp) {
      value[0] = '\0';
    }
    else {
      if (tmp->value) {
        strncpy(value, tmp->value, len-1);
      }
      else {
        value[0] = '\0';
      }
    }
  }
}

DefaultsList * _defaults_set(char *key, char *value) {
  DefaultsList *tmp;
  if (!key) return NULL;
  tmp = _defaults_get_key(key);
  if (!tmp) {
    tmp = malloc(sizeof(DefaultsList));
    assert(tmp);
    
    memset(tmp, 0, sizeof(DefaultsList));
    tmp->next = _defaults_list;
/*    strcpy(tmp->key, key);*/
    tmp->key = strdup(key);
    assert(tmp->key);
    
    _defaults_list = tmp;
  }
  if (value) {
    /*strcpy(tmp->value, value);*/
    tmp->value = strdup(value);
    assert(tmp->value);
  }
  else {
    tmp->value = NULL;
  }
  
  return tmp;
}

void defaults_add(char *key, char *value) {
  _defaults_set(key, value);
}

void defaults_update(char *key, char *value) {
  DefaultsList *tmp = _defaults_set(key, value);
  if (tmp) {
    tmp->flags |= DEFAULTS_LIST_FLAG_MODIFIED;
  }
}

DefaultsList * _defaults_get_key(char *key) {
  DefaultsList *tmp = _defaults_list;
  if (!key) return NULL;
  while (tmp) {
    if (!strcmp(tmp->key, key)) {
      return tmp;
    }
    tmp = tmp->next;
  }
  return NULL;
}
