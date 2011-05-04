/** @defgroup libhelper Helper functions for libmensa.
 *  @ingroup libmensa
 *  @file
 *  @ingroup libhelper
 *  Header file for helper functions.
 */
#ifndef __MENSA_HELPERS_H__
#define __MENSA_HELPERS_H__

/** List element for single linked list
 */
/* @{ */
typedef struct _mensaList mensaList;
struct _mensaList {
  void *data;            /**< Pointer to the data stored. */
  mensaList *next;       /**< Pointer to the next element in the list. */
};
/* @} */

mensaList *mensa_list_prepend(mensaList *list, void *data);
mensaList *mensa_list_reverse(mensaList *list);

void mensa_string_beautify(char *str, int inner);

#endif