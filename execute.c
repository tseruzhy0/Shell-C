/**
   This file contains all the function related to manipulating exec* commands
   as well as handling the pipeling
**/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include "list.h"

//global path variable
extern List path;
List path;

/**
   Handle our pipeline for n pipes
**/
void execve_pipe_handler(int argc, char **argv) {
  int newpfd[2];
  int oldpfd[2];
  char **argvtemp;
  char ***pipecmd;
  int i = 0;
  int k = 0;
  int l = 0;
  int newargs = 0;
  int storage = 0;
  int cmdstored = 1;
  int numcmds = 0;
  int pid;

  //Make the initial size of the array of pointers to argv arrays
  //larger and then later realloc
  pipecmd = (char ***) malloc(argc * sizeof(char***));

  //Step through each arg
  for (; i < argc; i++) {
    storage += strlen(argv[i]);

    //found a pipe put the previous commands into a temp argv
    if ( strcmp(argv[i], "|") == 0 || i == argc - 1) {

      //if its the last command increment the num of args so we dont lose anything
      if(i == argc - 1)
	newargs++;

      //alloc memory for our temp arv and make l the position of the command after
      //the pipe or at the beggining
      argvtemp = (char **) malloc(newargs * sizeof(char*));
      l = i - newargs;

      //make sure we didnt move back to far if so bump forward one
      if (strcmp(argv[l], "|") == 0)
	l++;

      //move the args from argv to to our temp argv 
      for (k = 0; k < newargs; k++) {
	argvtemp[k] = (char *) malloc(strlen(argv[l]) * sizeof(char));
	strcpy(argvtemp[k], argv[l]);
	l++;
      }

      //make sure that if there are any extra offset that its set to null
      argvtemp[k] = NULL;

      //realloc incase the initial size was off
      //assign our array of argv to the new argvtemp and then iterate our number of commands
      argvtemp = (char**) realloc(argvtemp, k * sizeof(char*));
      pipecmd[numcmds] = argvtemp;
      storage = 0;
      numcmds++;
      newargs = 0;
    }
    else {
      newargs++;
    }
  }

  //realloc our array of argv's to the correct size
  pipecmd = (char***) realloc(pipecmd, numcmds * sizeof(char**));

  //go through each command
  for (i = 0; i < numcmds; i++) {

    //if we have another command make a new pipe
    if (i+1 < numcmds)
      pipe(newpfd);

    //create a new procces
    pid = fork();
    
    if (pid == 0) { //child

      if ( i > 0) { //check for a previous command
	//if we had a previous command lets close its pipes
	//after we get its data
	close(oldpfd[1]);
	dup2(oldpfd[0], 0);
	close(oldpfd[0]);
      }
      if ( i+1 < numcmds) { //check for next command
	//if we have another command close pipe and send out our data into the pipe
	close(newpfd[0]);
	dup2(newpfd[1], 1);
	close(newpfd[1]);
      }
      //execute command or exit out of proccess
      execve_handler(0, pipecmd[i]);
      _exit(1);
    }
    else {//parent
      if ( i > 0) { //check for a previous command
	//if we had a process let us close its pipes
	close(oldpfd[0]);
	close(oldpfd[1]);
      }
      if ( i+1 <  numcmds) {//check for next command
	//if we have another command set up a pipe for it
	oldpfd[0] = newpfd[0];
	oldpfd[1] = newpfd[1];
      }
      //wait till we are dont with child process
      wait(NULL);
      //if we have nothing left wait for all the children then kill
      if (i+1 == numcmds) {
	wait(NULL);
	_exit(1);
      }
    } 
  }

  //backup
  if (i+1 == numcmds) {
    wait(NULL);
      _exit(1);
    }  
}

/**
   Handle everything to do with using execve
**/
int execve_handler(int argc, char **argv) {
  Node *traverse;
  char *fullname;
  char *pathname;
  char *prog;
  char *slash = "/\0";

  //lets get ready to iterate throught the list
  traverse = path.head;

  //set pathname to the size of a path from our list
  //copy that data to pathname and add a slash to the end of it
  pathname = (char *) malloc( strlen( (char *)traverse->data) * sizeof(char) + 2);
  strcpy(pathname, (char *) traverse -> data);
  pathname = strcat(pathname, slash);

  //get the program name
  prog = argv[0];
  
  //tac the program name onto the end of the pathname to get the full qualified name
  fullname = (char *) malloc( (strlen(pathname) + strlen(prog)) * sizeof(char)+2);
  fullname = strcat(pathname, prog);

  //exec here to make sure its not a local program
  execv(argv[0], (char* const*) argv);
  
  //lets go through the path variable linked list repeating what we did above
  while (traverse -> next != NULL) {
   
    execv(fullname, (char* const*) argv);
    
    traverse = traverse -> next;

    pathname = (char *) malloc( strlen( (char *)traverse->data) * sizeof(char) + 2);
    strcpy(pathname, (char *) traverse -> data);    
    pathname = strcat(pathname, slash);

    fullname = (char *) malloc( (strlen(pathname) + strlen(prog)) * sizeof(char) + 2);
    fullname = strcat(pathname, prog);


    execv(fullname, (char* const*) argv);
  }

  //need this if we only had one path in the path variable
  execv(fullname, (char* const*) argv);
  printf("command: %s, part1: %s, part2: %s", argv[0], argv[1], argv[2]);
  _exit(1);
}
