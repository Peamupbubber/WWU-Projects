/* CS 347 -- Mini Shell!
 * Original author Phil Nelson 2000
 */
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <ctype.h>
#include "argparse.h"
#include "builtin.h"

/* PROTOTYPES */
void processline (char *line);
ssize_t getinput(char** line, size_t* size);
void clean();

static int argCount;
static char** arguments;
static char* line;

//Ensures all allocated blocks are freed
void clean() {
  free(line);
  for(int i = 0; i < argCount; i++) {
    free(arguments[i]);
  }
  free(arguments);
}

/* main
 * This function is the main entry point to the program.  This is essentially
 * the primary read-eval-print loop of the command interpreter.
 */
int main () {
  atexit(clean);

  //Clear the terminal
  processline("clear");

  size_t size = sizeof(char);
  line = malloc(size * sizeof(char));

  while(1) {
    getinput(&line, &size);
    processline(line);
  }
  return EXIT_SUCCESS;
}


/* getinput
* line     A pointer to a char* that points at a buffer of size *size or NULL.
* size     The size of the buffer *line or 0 if *line is NULL.
* returns  The length of the string stored in *line.
*
* This function prompts the user for input, e.g., %myshell%.  If the input fits in the buffer
* pointed to by *line, the input is placed in *line.  However, if there is not
* enough room in *line, *line is freed and a new buffer of adequate space is
* allocated.  The number of bytes allocated is stored in *size. 
* Hint: There is a standard i/o function that can make getinput easier than it sounds.
*/
ssize_t getinput(char** line, size_t* size) {
  ssize_t len = 0;

  printf("myshell$ ");
  
  if((len = getline(line, size, stdin)) < 0) {
    perror("Error reading input");
  }
  //Removes newline character from input string
  len--;
  line[0][len] = '\0';

  if(!len) {
    *size = 0;
  }
  return len;
}


/* processline
 * The parameter line is interpreted as a command name.  This function creates a
 * new process that executes that command.
 * Note the three cases of the switch: fork failed, fork succeeded and this is
 * the child, fork succeeded and this is the parent (see fork(2)).
 * processline only forks when the line is not empty, and the line is not trying to run a built in command
 */
void processline (char *line)
{
 /*check whether line is empty*/
  if(strlen(line)) {
    pid_t cpid;
    int   status;
    arguments = argparse(line, &argCount);

    /*check whether arguments are builtin commands
    *if not builtin, fork to execute the command.
    */
    if(!builtIn(arguments, argCount)) {
      cpid = fork();

      //Parent
      if(cpid > 0) {
        wait(&status);
      }

      //Child
      else if(cpid == 0) {
        //Find the path to the executable command if it exists
        char* path = getenv("PATH");
        int size;
        char* path_ptr;

        while(*path != '\0') {
          size = 0;
          path_ptr = path;
          while(*path != ':' && *path != '\0') {
            size++;
            path++;
          }

          char path_to_check[size+strlen(arguments[0])+2];
          strncpy(path_to_check, path_ptr, size);
          strcpy(path_to_check+size, "/");
          strncpy(path_to_check+size+1, arguments[0], strlen(arguments[0]));
          path_to_check[size+strlen(arguments[0])+1] = '\0';

          path++;

          if(!access(path_to_check, F_OK)) {
            execv(path_to_check, arguments);
          }
        }
        printf("Command not found\n");
        exit(EXIT_FAILURE);
      }

      else
        printf("Fork failed\n");
    }

    //Free the arguments after each use
    for(int i = 0; i < argCount; i++) {
      free(arguments[i]);
    }
    free(arguments);
  }

  else
    free(line);
}