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
  unsigned char *key;
  unsigned char *value;
  unsigned long flags;
  DefaultsList *next;
};

DefaultsList *_defaults_list = NULL;

DefaultsList * _defaults_set(unsigned char *key, unsigned char *value);
DefaultsList * _defaults_get_key(unsigned char *key);
int _defaults_rc_read_line(FILE *stream);
void _defaults_rc_skip_spaces(FILE *stream);

int defaults_read(unsigned char *filename) {
  FILE *f;
  if (!filename) {
    return 1;
  }
  
  if ((f = fopen(filename, "r")) == NULL) {
    fprintf(stderr, "Could not read rc file \"%s\": ", filename);
    perror("");
    return 1;
  }
  
  while (!feof(f)) {
    if (_defaults_rc_read_line(f) != 0) {
      fprintf(stderr, "Error parsing rc file.\n");
      fclose(f);
      return 1;
    }
  }  
  
  fclose(f);  
  return 0;
}

void _defaults_rc_skip_spaces(FILE *stream) {
  int tok;
  while (1) {
    tok = fgetc(stream);
    if (feof(stream)) {
      return;
    }
    if (tok != ' ' && tok != '\t') {
      fseek(stream, -1L, SEEK_CUR); /*ungetc(tok, stream);*/
      return;
    }
  }
}

int _defaults_rc_read_line(FILE *stream) {
  int tok;
  unsigned char *key_buf;
  unsigned char *val_buf;
  int key_len;
  int val_len;
  int i;
  long key_start;
  long val_start;
  long last_non_space;
  long eol = -1;

  _defaults_rc_skip_spaces(stream);
  key_start = ftell(stream);

  tok = fgetc(stream);  /* if we reach eof with seek cur, eof-flag is not set yet *
                         * so we try to read the token here                       */

  if (feof(stream)) {
    return 0;
  }
  if (tok == '#') { /* this is a comment: eat till end of line/file */
    do {
      tok = fgetc(stream);
    } while (!feof(stream) && tok != '\n');
    return 0;
  }
  else if (tok == '\n') {  /* already reached end of line */
    return 0;
  }
  else {
    key_len = 0;
    do {
      key_len++;
      tok = fgetc(stream);
      /* the file should not end here and there should be no newline*/
      if (feof(stream) || tok == '\n') {
        return 1;
      }
    } while (tok != ' ' && tok != '=' && tok != '\t');
    
    /* if we haven't read the = yet, skip spaces and try to read it*/
    if (tok != '=') {
      _defaults_rc_skip_spaces(stream);
      tok = fgetc(stream);
      /* next token no = so return an error */
      if (tok != '=') {
        return 1;
      }
    }
    _defaults_rc_skip_spaces(stream);
    
    val_start = ftell(stream);
    val_len = 0;
    last_non_space = val_start;
    while (!feof(stream)) {
      tok = fgetc(stream);
      if (tok != '\n') {
        val_len++;
        if (tok != ' ' && tok != '\t') {
          last_non_space = val_len;
        }
      }
      else {
        eol = ftell(stream);
        break;
      }
    }
    /* cut of trailing spaces */
    val_len = last_non_space;
    
    key_buf = malloc(sizeof(unsigned char)*(key_len+1));
    assert(key_buf);
    
    val_buf = malloc(sizeof(unsigned char)*(val_len+1));
    assert(val_buf);
    
    /* goto beginning of key */
    fseek(stream, key_start, SEEK_SET);
    for (i = 0; i < key_len; i++) {
      key_buf[i] = (unsigned char)fgetc(stream);
    }
    key_buf[key_len] = '\0';
    
    /* goto beginning of value */
    fseek(stream, val_start, SEEK_SET);
    for (i = 0; i < val_len; i++) {
      val_buf[i] = (unsigned char)fgetc(stream);
    }
    val_buf[val_len] = '\0';
    
    /* find end of line*/
    if (eol == -1) {
      /* no eol, was eof */
      fseek(stream, 1L, SEEK_END);
    }
    else {
      fseek(stream, eol, SEEK_SET);
    }
    
    /* insert to defaults */
    defaults_add(key_buf, val_buf);
    free(key_buf);
    free(val_buf);
    return 0;
  }
}

int defaults_write(unsigned char *filename) {
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

void defaults_get(unsigned char *key, unsigned char *value, int len) {
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

DefaultsList * _defaults_set(unsigned char *key, unsigned char *value) {
  DefaultsList *tmp;
  if (!key) return NULL;
  tmp = _defaults_get_key(key);
  if (!tmp) {
    tmp = malloc(sizeof(DefaultsList));
    assert(tmp);
    
    memset(tmp, 0, sizeof(DefaultsList));
    tmp->next = _defaults_list;
    tmp->key = strdup(key);
    assert(tmp->key);
    
    _defaults_list = tmp;
  }
  if (value) {
    tmp->value = strdup(value);
    assert(tmp->value);
  }
  else {
    tmp->value = NULL;
  }
  
  return tmp;
}

void defaults_add(unsigned char *key, unsigned char *value) {
  _defaults_set(key, value);
}

void defaults_update(unsigned char *key, unsigned char *value) {
  DefaultsList *tmp = _defaults_set(key, value);
  if (tmp) {
    tmp->flags |= DEFAULTS_LIST_FLAG_MODIFIED;
  }
}

DefaultsList * _defaults_get_key(unsigned char *key) {
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
