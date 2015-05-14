#ifndef LIST_H
#define LIST_H

#include <stdio.h>
#include <stdlib.h>

typedef struct LinkedNode {
  void *data;
  struct LinkedNode *next;
  struct LinkedNode *prev;
}Node;

typedef struct LinkedList {
  int size;
  int  (*match) (const void *key1, const void *key2);
  void (*destroy) (void *data);

  Node *head;
  Node *tail;
}List;

int compare(void *var1, void *var2);

void string_format(void *data);

void print(List *list, void (*string_format) (void *));

void list_init(List *list, void (*destroy) (void *data));

void list_destroy(List *list);

int list_ins_next(List *list, Node *element, const void *data);

int list_rem_next(List *list, Node *element, void **data);

#define list_size(list) ((list) -> size)

#define list_head(list) ((list) -> head)

#define list_tail(list) ((list) -> tail)

#define list_is_head(list, element) ((element) == (list) -> head ? 1 : 0)

#define list_is_tail(element) ((element) -> next == NULL ? 1 : 0)

#define list_data(element) ((element) -> data)

#define list_next(element) ((element) -> next)

#endif
