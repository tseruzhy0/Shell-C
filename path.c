/**
   This group of function is used for interacting with
   the linked list with the idea of using it as a path variable
**/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "list.h"

extern List path;
List path;

//required for list interface not used
int compare(void *var1, void *var2) {
  char *str1 = var1;
  char *str2 = var2;
  
  return (strcmp(str1, str2));
}

//required by interface used to print out the path variable
void string_format(void *data) {
  char *str = data;
  
  printf("%s:", str);
}

//add to the path list
void path_add(char **argv) {
  //printf("%d\n", path.size);
  list_ins_next(&path, NULL, argv[2]);
}

//deletes from the path list
int path_del(char **argv) {
  Node *temp;
  temp = path.head;
  int found = -1;
  int test = 5;

  //search the list for the path and pass the prev node to the
  //delete next function
  while (!list_is_tail(temp)) {
    found = (compare(argv[2], temp -> data));
    if (found == 0) {
      if(list_is_head(&path, temp))
	return list_rem_next(&path, NULL, (void **) &argv[2]);
      else
	return list_rem_next(&path, temp -> prev, (void **) &argv[2]);
    }
    temp = temp -> next;
  }
  return -1;
}

//print the path variable and pass our print function
void path_print() {
  print(&path, &string_format);
  //printf("%d\n",path.size);
}
  
//Manage our path functions
void path_handler(int argc, char **argv) {
  char *add = "+";
  char *del = "-";
  //printf("made it to print handler");
  if (argc <= 1) {
    path_print();
  }
  else {
    if ((strcmp(argv[1], add) ==0)) {
      path_add(argv);
    }
    else {
      if ((strcmp(argv[1], del) == 0)) {
	path_del(argv);
      }
      else {
	printf("Illegal argument\n");
      }
    }
  }
}
