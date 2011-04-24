#include "mensa_helpers.h"
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
