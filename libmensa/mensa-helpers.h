#ifndef __MENSA_HELPERS_H__
#define __MENSA_HELPERS_H__

typedef struct _mensaList mensaList;
struct _mensaList {
  void *data;
  mensaList *next;
};

mensaList *mensa_list_prepend(mensaList *list, void *data);
mensaList *mensa_list_reverse(mensaList *list);

#endif