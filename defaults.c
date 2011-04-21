#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>

#include "defaults.h"

typedef struct _DefaultsList DefaultsList;
struct _DefaultsList {
  char key[32];
  char value[256];
  DefaultsList *next;
};

DefaultsList *_defaults_list = NULL;

DefaultsList * _defaults_key_exists(char *key);

int defaults_read(char *filename) {
  /* if filename is NULL use home-folder*/
  return 0;
}

void defaults_get(char *key, char *value) {
  DefaultsList *tmp = _defaults_key_exists(key);
  if (value) {
    if (!tmp) {
      value[0] = '\0';
    }
    else {
      strcpy(value, tmp->value);
    }
  }
}

void defaults_add(char *key, char *value) {
  DefaultsList *tmp = _defaults_key_exists(key);
  if (!key) return;
  if (!tmp) {
    tmp = malloc(sizeof(DefaultsList));
    if (!tmp) {
      exit(1);
    }
    memset(tmp, 0, sizeof(DefaultsList));
    tmp->next = _defaults_list;
    strcpy(tmp->key, key);
    _defaults_list = tmp;
  }
  if (value) {
    strcpy(tmp->value, value);
  }
}

DefaultsList * _defaults_key_exists(char *key) {
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
