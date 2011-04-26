#ifndef __MENSA_OUTPUT_H__
#define __MENSA_OUTPUT_H__

#include <stdio.h>

void mensa_output_fixed_len_str(FILE *stream, char *str, int len);

void mensa_output_block(FILE *stream, char *str, int length,
                        int indent, int indent_first_line);

#endif