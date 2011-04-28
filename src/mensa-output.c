#include "mensa-output.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <curses.h>
#include <term.h>
#ifdef unix
static char term_buffer[2048];
#else
#define term_buffer 0
#endif

/* print a fixed length string, taking care of utf-8 characters
 * these are assumed to be correct */
void mensa_output_fixed_len_str(FILE *stream, char *str, int len) {
  int k = 0, l = 0;
  FILE *s = stream ? stream : stdout;
  if (!str) {
    for (k = 0; k < len; k++) {
      fputc(' ', s);
    }
    return;
  }
  while (str[k] != '\0') {
    if ((str[k] & 0x80) == 0) {
      fputc(str[k++], s);
    }
    else if ((str[k] & 0xe0) == 0xc0) {
      fputc(str[k++], s);
      fputc(str[k++], s);
    }
    else if ((str[k] & 0xf0) == 0xe0) {
      fputc(str[k++], s);
      fputc(str[k++], s);
      fputc(str[k++], s);
    }
    else if ((str[k] & 0xf8) == 0xf0) {
      fputc(str[k++], s);
      fputc(str[k++], s);
      fputc(str[k++], s);
      fputc(str[k++], s);
    }
    l++;
  }
  for (; l < len; l++) {
    fputc(' ', stream);
  }
}

int _mensa_output_block_line(FILE *stream, char *str, int length);

/** output a block of text of fixed line width
 */
void mensa_output_block(FILE *stream, char *str, int length,
                        int indent, int indent_first_line) {
  int offset = 0;
  int delta = 0;  
  int len = 0;
  FILE *s = stream ? stream : stdout;
  int k;
  
  if (!str) {  /* no string given, just print newline */
    fputc('\n', s);
    return;
  }
  len = strlen(str);
  while (offset < len) {
    if (offset > 0 || (offset == 0 && indent_first_line)) {
      for (k = 0; k < indent; k++) {
        fputc(' ', s);
      }
    }
    delta = _mensa_output_block_line(s, &str[offset], length);
    if (delta == 0) {
      break;
    }
    offset += delta;
  }
}

/** writes up to length characters of str to stream
 *  @return offset of the next line */
int _mensa_output_block_line(FILE *stream, char *str, int length) {
  int len;
  int i,d;
  int last_space;
  int offset = 0;
  FILE *s = stream ? stream : stdout;
  
  /* skip leading spaces but be aware of the complete offset; needs work */
  while (str[offset] == ' ') { offset++; }
  if (offset) str = &str[offset];

  len = strlen(str);
  if (len <= length) { /* nothing to cut of, write to output */
    for (i = 0; str[i] != '\0'; i++) {
      fputc(str[i], s);
    }
    fputc('\n', s);
    return len+offset;
  }
  
  i = 0; d = 0;
  last_space = 0;
  /* could be done simpler, but we want to take care of utf-8 characters */
  while (i < length && str[i+d] != '\0') {
    if (str[i+d] == ' ' || str[i+d] == ',' || str[i+d] == '-' ||
        str[i+d] == ';' || 
         (str[i+d] == '.' && (str[i+d+1] != ',' && str[i+d+1] != ';'
          && str[i+d+1] != '-'))
        || str[i+d] == ':') {
      last_space = i+d;
    }
    else if ((str[i+d] & 0xe0) == 0xc0) {
      d++;
    }
    else if ((str[i+d] & 0xf0) == 0xe0) {
      d += 2;
    }
    else if ((str[i+d] & 0xf8) == 0xf0) {
      d+= 3;
    }
    i++;
  }
  if (last_space == 0) { /* no spaces up to length, cut of */
    last_space = i+d-1; /* -1 ? */
  }

  for (i = 0; i <= last_space; i++) {
    fputc(str[i], s);
  }
  fputc('\n', s);
  return last_space+1+offset;
}

int mensa_output_get_term_width(void) {
  char *termtype = getenv("TERM");
  if (termtype == NULL) {
    return -1;
  }
  
  if (tgetent(term_buffer, termtype) <= 0) {
    return -1;
  }
  
  return tgetnum("co");
}
