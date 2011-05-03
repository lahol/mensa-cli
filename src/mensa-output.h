/** @defgroup output Handle output to terminal.
 *  @ingroup mensacli
 *  @file
 *  @ingroup output
 *  Header file for output to terminal.
 */
#ifndef __MENSA_OUTPUT_H__
#define __MENSA_OUTPUT_H__

#include <stdio.h>

void mensa_output_fixed_len_str(FILE *stream, char *str, int len);

void mensa_output_block(FILE *stream, char *str, int length,
                        int indent, int indent_first_line);
                        
int mensa_output_get_term_width(void);

#endif
