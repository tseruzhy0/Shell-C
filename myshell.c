/*
  Project: myshell
  Author: John P Beres
  Description: Create a basic shell that will execute basic commands and manage
  its own path variable. Handle n pipes as well.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include "list.h"

/**
  Create a global variable for our path variable
**/
extern List path;
List path;

/**
   Handle the I/O redirection
   ***Note check and see if we can fix pipe io redirection
**/
void change_io(char **argr) {
  char *input = "<";
  char *output = ">";
  if ( strcmp(argr[0], output) == 0 ) {
    freopen(argr[1], "w+", stdout);
  }
  else {
    if ( strcmp(argr[0], input) == 0 ) {
      freopen(argr[1], "r", stdin);
    }
  }
}

/**
   After the line is parsed we will go through it and
   check for any of the commands supported
**/
void cmd_handler(int ispipe, int iochange, char argc, char **argv, char **argr) {
  //Constants for our built in commands
  char *path = "path";
  char *exit = "exit";
  char *cd = "cd";

  //Check for each command
  if ( (strcmp(path, argv[0]) == 0) ) { //Path Command
    path_handler(argc, argv);
  }
  else {
    if ( (strcmp(exit, argv[0]) == 0) ) { //Exit Command
      _exit(1);
    }
    else {
      if ((strcmp(cd, argv[0]) == 0)) { //cd Command (change directory)
	chdir(argv[1]);
      }
      else {
	//if its not a built in command and we have a command
	//we will fork and try to execute commands associated
	//with each path
	if (argc > 0) {
	  if (fork() == 0) {
	    if(iochange)
	      change_io(argr);
	    if(ispipe)
	      execve_pipe_handler(argc, argv);
	    else
	      execve_handler(argc, argv);
	  }
	  else {
	    wait(NULL);
	  }
	}
      }
    }
  }
}

/**
   Parse the line based on spaces
   also throw flags for I/O redirection and pipe()
**/
void parse_line(int *argc, int *iochange, int *ispipe, char ***args, char ***argr) {
  char input[4096];
  char *line;
  char *tempArgs;
  char *removeNewLine;
  int numOfArgs = 0;
  int numOfArgr = 0;
  int redirection = 0;
  
  //gets user input
  line = fgets(input, 4096, stdin);

  //if nothing was entered flush and return to the main loop
  if(strcmp(line, "\n") == 0) {
    fflush(stdout);
    fflush(stdin);
    return;
  }

  //search for the carriage return and replace it with \0
  removeNewLine = strchr(input, '\n');
  if(removeNewLine) *removeNewLine = '\0';
  
  //allocate memory for arguments array
  //and redirection arg
  *args = malloc( (strlen(line)) * sizeof(char*) );
  *argr = malloc( (strlen(line)) * sizeof(char*) );
  
  //tokenize the input
  for (tempArgs = strtok(line, " ");
       tempArgs != NULL;
       tempArgs = strtok(NULL, " ") ) {

    //check for redirection
    if (strcmp(tempArgs, "<") == 0 ||
	strcmp(tempArgs, ">") == 0)
      redirection = 1;

    //check for pipes
    if ( strcmp(tempArgs, "|") == 0 )
      *ispipe = 1;

    //if its not a redirection argument put in normal args array
    //else put the arguments in the redirection args array
    if(!redirection) {
      (*args)[numOfArgs] = tempArgs;
      numOfArgs++;
    }
    else {
      (*argr)[numOfArgr] = tempArgs;
      numOfArgr++;
    }
  }
  
  //re-allocate the memory incase intial allocation was to large
  *args = (char**) realloc(*args, (numOfArgs+1) * sizeof(char*));
  *argr = (char**) realloc(*argr, numOfArgs * sizeof(char*));

  (*args)[numOfArgs] = NULL;
  
  *argc = numOfArgs;
  *iochange = redirection;
  
}

/**
   the main() will house the main loop and everything else
   is branched out from here
**/
int main() {
  char *shelldir;
  char *path1 = "/usr/bin\0";
  char *path2 = "/bin\0";
  char **argv;
  char **argr;
  int argc = 0;
  int iochange = 0;
  int ispipe = 0;

  //Set up our path variable with some basic paths
  list_init(&path, free);
  list_ins_next(&path, NULL , path1);
  list_ins_next(&path, NULL , path2);
  
  //get the current workign directory and add it to the
  //path variable for any native shell apps
  shelldir = getcwd(NULL,0);
  list_ins_next(&path, NULL, shelldir);

  //Main Loop
  while(1) {
    
    printf("<myshell>");
     
    parse_line(&argc, &iochange, &ispipe,  &argv, &argr);

    cmd_handler(ispipe, iochange, argc, argv, argr);

    //reset flags and arg count
    ispipe = 0;
    iochange = 0;
    argc = 0;      
  }
}
