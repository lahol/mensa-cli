/** @defgroup utils Utilities
 *  @ingroup mensacli
 *  @file
 *  @ingroup utils
 *  Header file for utilities used by the command line interface.
 */
#ifndef __UTILS_H__
#define __UTILS_H__

int utils_file_exists(const unsigned char *filename);
char * utils_get_rcpath(void);

/** @brief Single linked list.
 */
/* @{ */
typedef struct _UtilsList UtilsList;
struct _UtilsList {
  void *data;          /**< Pointer to the data stored in the list. */
  UtilsList *next;     /**< Pointer to the next list element. */
};
/* @} */

UtilsList *utils_list_prepend(UtilsList *list, void *data);

#endif
