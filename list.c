/**
   this is my linked list
   i use a void pointer and require a function pointer
   for any function that requires interaction with the data
**/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"

//intialize the array
void list_init(List *list, void (*destroy)(void *data)) {
  list -> size = 0;
  list -> destroy  = destroy;
  list -> head = NULL;
  list -> tail = NULL;
  
  return;
}

//free all the memory from the list
void list_destroy(List *list) {
  void *data;

  while(list_size(list) > 0) {
    if (list_rem_next(list, NULL, (void **)&data) == 0 && list -> destroy != NULL) {
      list -> destroy(data);
    }
  }

  memset(list , 0 , sizeof(List));

  return;
}

//will insert either at the head of the list or at a specific spot
//specified by the element passed (NULL is for inserting at the head of the list
int list_ins_next(List *list , Node *element , const void *data) {

  Node *new_element;
  Node *temp;

  if ((new_element = (Node *)malloc(sizeof(Node))) == NULL)
    return -1;

  new_element -> data = (void *) data;

  if (element == NULL) {

    if (list_size(list) == 0) {
      list -> tail = new_element;
      list -> head = new_element;
    }
    else {
      new_element -> next = list -> head;
      list -> head -> prev = new_element;
      list -> head = new_element;
    }
  }
  else {
    
    if (element -> next == NULL
	&& list_size(list) == 0) {
      
      list -> tail = new_element;
      list -> head = new_element;
    }
    else {
      new_element -> next = element -> next;
      element -> next = new_element;
      new_element -> prev = element;
    }
  }

  list -> size++;

  return 0;
}

//delete node works similiar to the insertion
int list_rem_next(List *list, Node *element, void **data) {
  Node *old_element;

  if (list_size(list) == 0)
    return -1;

  if (element == NULL) {

    *data = list -> head -> data;
    old_element = list -> head;
    list -> head = list -> head -> next;
    list -> head -> prev = NULL;

    if (list_size(list) == 1)
      list -> tail = NULL;
  }
  else {

    if (element -> next == NULL)
      return -1;

    *data = element -> next -> data;
    old_element = element -> next;
    element -> next = element -> next -> next;
    element -> next -> prev =  element;

    if (element -> next == NULL)
      list -> tail = element;

  }

  free(old_element);

  list -> size--;

  return 0;
}

//print out the contents fo the linked list
//requires a funtion to be passed that does the actual printing
void print(List *list, void (*string_format) (void *)) {
  Node *temp;
  void *data;

  temp = list -> head;

  
  while (temp -> next != NULL) {
    data =  list_data(temp);
    (string_format)(data);
    temp = temp -> next;
  }

  data = list_data(temp);
  (string_format)(data);
}
