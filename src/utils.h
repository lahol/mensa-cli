#ifndef __UTILS_H__
#define __UTILS_H__

int utils_file_exists(const unsigned char *filename);
char * utils_get_rcpath(void);

/* single linked list */
typedef struct _UtilsList UtilsList;
struct _UtilsList {
  void *data;
  UtilsList *next;
};

UtilsList *utils_list_prepend(UtilsList *list, void *data);

#endif