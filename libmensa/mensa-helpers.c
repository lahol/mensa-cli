#include "mensa-helpers.h"
#include <stdlib.h>
#include <string.h>

mensaList *mensa_list_prepend(mensaList *list, void *data) {
  mensaList *ins = malloc(sizeof(mensaList));
  ins->next = list;
  ins->data = data;
  return ins;
}


/* TODO!! */
mensaList *mensa_list_reverse(mensaList *list) {
  return list;
}

char _mensa_next_nonws(char *str, int *offset);

/** removes leading and trailing whitespaces; double whitespaces
 *  and whitespaces in front of ,.; if inner is true
 */
void mensa_string_beautify(char *str, int inner) {
  char *buffer = NULL;
  int len, i, d;
  int spaceflag;
  char tok;
  int t;
  
  if (!str) {
    return;
  }
  /* remove trailing whitespaces */  
  len = strlen(str);
  for (i = len-1; i >= 0; i--) {
    if (str[i] == ' ' ||
        str[i] == '\t' ||
        str[i] == 0xd ||
        str[i] == 0xa) {
      str[i] = '\0';
    }
    else if (str[i] == (char)0xa && i > 0 && buffer[i-1] == (char)0xc2) {
      /* no-break space, U+00A0 (0xc2 0xa0)
       * needs type conversion because a0 < 0 for signed char */
      str[i] = str[i-1] = '\0';
      i--;
    }
    else {
      break;
    }
  }
  /* remove leading whitespaces */
  d = 0;
  do {
    if (str[d] != ' ' &&
        str[d] != '\t' &&
        str[d] != 0xa &&
        str[d] != 0xd) {
      if (str[d] != (char)0xc2 || str[d+1] != (char)0xa0) {
        break;
      }
      else {
       d++;
      }
    }
    d++;
  } while (str[d] != '\0');
  
  /* move string */
  if (d > 0) {
    i = 0;
    while (str[i+d] != '\0') {
      str[i] = str[i+d];
      i++;
    }
    str[i] = '\0';
  }
  
  if (!inner) {
    return;
  }
  
  /* do the inner stuff */
  /* copy buffer and write to str */
  len = strlen(str);
  buffer = strdup(str);
  if (!buffer) return;
  i = 0;
  d = 0;
  spaceflag = 0;
  while (buffer[i+d] != '\0') {
    if (buffer[i+d] == ' ' ||
        buffer[i+d] == '\t' ||
        buffer[i+d] == 0xa ||
        buffer[i+d] == 0xd) {
      tok = _mensa_next_nonws(&buffer[i+d], &t);
      if (tok == ',' || tok == '.' || tok == ';' || tok == ':') {
        d += t;
      }
      else if (!spaceflag) {
        str[i] = ' ';
        spaceflag = 1;
        i++;
      }
      else {
        d++;
      }
    }
    else if (buffer[i+d] == 0xc2 && buffer[i+d+1] == 0xa0) {
      tok = _mensa_next_nonws(&buffer[i+d], &t);
      if (tok == ',' || tok == '.' || tok == ';' || tok == ':') {
        d += t;
      }
      else if (!spaceflag) {
        str[i] = ' ';
        spaceflag = 1;
        i++; d++;
      }
      else {
        d += 2;
      }
    }
    else {
      spaceflag = 0;
      str[i] = buffer[i+d];
      i++;
    }
  }
  str[i] = '\0';
}

char _mensa_next_nonws(char *str, int *offset) {
  int k = 0;
  if (!str) return 0;
  while (str[k] != '\0') {
    if (str[k] != ' ' &&
        str[k] != '\t' &&
        str[k] != 0xa &&
        str[k] != 0xd) {
      if (str[k] != 0xc2 || str[k+1] != 0xa0) {
        if (offset) *offset = k;
        return str[k];
      }
      else {
        k++;
      }
    }
    k++;
  }
  if (offset) *offset = k;
  return 0;
}
