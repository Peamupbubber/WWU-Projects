#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "argparse.h"
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>

#define FALSE (0)
#define TRUE  (1)

/*
* argCount is a helper function that takes in a String and returns the number of "words" in the string assuming that whitespace is the only possible delimeter.
*/
static int argCount(char* line) {
  int count = 0;
  int b = TRUE;

  while(*line) {
    if((*line == ' ' || *line == '\t') && !b)
      b = TRUE;
    if(*line != ' ' && *line != '\t' && b) {
      count++;
      b = FALSE;
    }
    line++;
 }
 return count;
}

/*
* Argparse takes in a String and returns an array of strings from the input.
* The arguments in the String are broken up by whitespaces in the string.
* The count of how many arguments there are is saved in the argcp pointer
*/
char** argparse(char* line, int* argcp) {
  *argcp = argCount(line);
  char** args = malloc((*argcp + 1)* sizeof(char*));

  for(int i = 0; i < *argcp; i++) {
    char* line_ptr = line;  
    int size = 0;

    while(*line != ' ' && *line != '\t' && *line != '\0') {
      size++;
      line++;
    }
    while(*line == ' ' || *line == '\t') {
      line++;
    }

    args[i] = malloc(size + 1);
    strncpy(args[i], line_ptr, size);
    args[i][size] = '\0';
  }

  args[*argcp] = NULL;
  return args;
}
