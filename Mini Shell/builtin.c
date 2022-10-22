#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>
#include "builtin.h"
#include <string.h>
#include <dirent.h>
#include <time.h>
#include <pwd.h>
#include <grp.h>

#define NUM_COMMANDS (6)

/* PROTOTYPES */
static void exitProgram(char** args, int argcp);
static void cd(char** args, int argpcp);
static void pwd(char** args, int argcp);
static void ls(char** args, int argcp);
static void cp(char** args, int argcp);
static void touch(char** args, int argcp);


/* builtIn
 ** built in checks each built in command the given command, if the given command
 * matches one of the built in commands, that command is called and builtin returns 1.
 *If none of the built in commands match the wanted command, builtin returns 0;
  */
int builtIn(char** args, int argcp) {
  char* builtin[] = {"exit", "pwd", "cd", "ls", "cp", "touch"};
  int to_return = 0;

  //Tests if the given argument is a builtin command
  for(int i = 0; i < NUM_COMMANDS; i++) {
    if(args[0] != NULL && !strcmp(args[0], builtin[i])) {
      to_return = i + 1;
    }
  }

  switch(to_return) {
    case 1:
      exitProgram(args, argcp);
      break;
    
    case 2:
      pwd(args, argcp);
      break;

    case 3:
      cd(args, argcp);
      break;

    case 4:
      ls(args, argcp);
      break;

    case 5:
      cp(args, argcp);
      break;

    case 6:
      touch(args, argcp);
      break;
  }

  return !!to_return;
}

//exit: exits the program with the given value or 0
static void exitProgram(char** args, int argcp) {
  if(argcp > 2)
    printf("Too many arguments\n");

  else if(argcp == 2) {
    //Ensure argument is a number
    int b = 1;
    for(int i = 0; i < strlen(args[1]); i++) {
      if(!isdigit(args[1][i])) {
        b = 0;
      }
    }
    if(b)
      exit(atoi(args[1]));
    else
      printf("Invalid argument\n");
  }
  else
    exit(EXIT_SUCCESS);
}

//pwd: prints the current working directory assuming it is the only argument given
static void pwd(char** args, int argcp) {
  if(argcp > 1)
    printf("Too many arguments\n");

  else {
    //Get the max length of the path name
    long path_name_length = pathconf("/", _PC_NAME_MAX);
    char buf[path_name_length];

    printf("%s\n", getcwd(buf, sizeof(buf)));
  }
}

//cd: ensures correct number of arguments are given then runs chdir() on either the given directory or the /home dir if nothing is given
static void cd(char** args, int argcp) {
  if(argcp > 2)
    printf("Too many arguments\n");
  else if(argcp == 1)
    chdir("/home");
  else {
    DIR* dir;
    if(!access(args[1], F_OK)) {
      if((dir = opendir(args[1])) != NULL) {
        chdir(args[1]);
        if(closedir(dir) < 0) {
          perror("Error closing file");
        }
      }
      else
        printf("Not a directiory\n");
    }
    else
      printf("Cannot find directory\n");
  }
}

//ls: ensures 1 or 2 arguments are given and that -l is the tag if one is given, then runs the specified command
static void ls(char** args, int argcp) {
  if(argcp > 2)
    printf("Too many arguments\n");

  else if(argcp == 2 && strcmp(args[1], "-l"))
    printf("Invalid tag. Valid tag: -l\n");

  else {
    //ls case
    if(argcp == 1) {
      int dir_count = 0;
      DIR* dir = opendir(".");
      struct dirent* dirp;

      while((dirp = readdir(dir)) != NULL) {
        if((dirp->d_name)[0] == '.')
          continue;
        printf("%s  ", dirp->d_name);
        dir_count++;
      }
      if(closedir(dir) < 0) {
        perror("Error closing file");
      }
      if(dir_count)
        printf("\n");
    }
    
    //ls -l case
    //Creates a char*** to store all of the information so that it can be sorted and printed out in a correctly formatted way
    else {
      DIR* dir = opendir(".");
      struct dirent* dirp;
      int dir_count = 0;

      while((dirp = readdir(dir)) != NULL) {
        if((dirp->d_name)[0] == '.')
          continue;
        dir_count++;
      }
      if(closedir(dir) < 0) {
        perror("Error closing file");
      }

      dir = opendir(".");

      char*** all_info = malloc(dir_count * sizeof(char**));
      int loc = 0;
      int total_blocks = 0;

      while((dirp = readdir(dir)) != NULL) {

        if((dirp->d_name)[0] == '.')
          continue;

        all_info[loc] = malloc(9 * sizeof(char*));
        struct stat s;
        lstat(dirp->d_name, &s);
      
        //Adds the blocks to the total
        total_blocks += s.st_blocks;

        //Mode and file permissions
        char permissions[10];

        permissions[0] =  S_ISDIR(s.st_mode)   ? 'd' : '-';
        permissions[1] = (S_IRUSR & s.st_mode) ? 'r' : '-';
        permissions[2] = (S_IWUSR & s.st_mode) ? 'w' : '-';
        permissions[3] = (S_IXUSR & s.st_mode) ? 'x' : '-';
        permissions[4] = (S_IRGRP & s.st_mode) ? 'r' : '-';
        permissions[5] = (S_IWGRP & s.st_mode) ? 'w' : '-';
        permissions[6] = (S_IXGRP & s.st_mode) ? 'x' : '-';
        permissions[7] = (S_IROTH & s.st_mode) ? 'r' : '-';
        permissions[8] = (S_IWOTH & s.st_mode) ? 'w' : '-';
        permissions[9] = (S_IXOTH & s.st_mode) ? 'x' : '-';

        all_info[loc][0] = malloc(strlen(permissions) + 1);
        strcpy(all_info[loc][0], permissions);
        all_info[loc][0][strlen(permissions)] = '\0';


        //Number of links
        int links = s.st_nlink;
        int links_digits = 0;
        while(links) {
          links_digits++;
          links /= 10;
        }
        all_info[loc][1] = malloc(links_digits+1);
        sprintf(all_info[loc][1], "%ld", s.st_nlink);
        all_info[loc][1][links_digits] = '\0';
        

        //User and group ids
        struct passwd* pw = getpwuid(s.st_uid);
        all_info[loc][2] = malloc(strlen(pw->pw_name)+1);
        strcpy(all_info[loc][2], pw->pw_name);
        all_info[loc][2][strlen(pw->pw_name)] = '\0';
        endpwent();


        struct group* gp = getgrgid(s.st_gid);
        all_info[loc][3] = malloc(strlen(gp->gr_name)+1);
        strcpy(all_info[loc][3], gp->gr_name);
        all_info[loc][3][strlen(gp->gr_name)] = '\0';
        endgrent();


        //File size
        int size = s.st_size;
        int size_digits = 0;
        while(size) {
          size_digits++;
          size /= 10;
        }
        all_info[loc][4] = malloc(size_digits+1);
        sprintf(all_info[loc][4], "%ld", s.st_size);
        all_info[loc][4][size_digits] = '\0';


        //Time of last modification
        struct tm* time = localtime(&s.st_mtime);
        char* months[] = {"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};
        
        all_info[loc][5] = malloc(strlen(months[time->tm_mon])+1);
        strcpy(all_info[loc][5], months[time->tm_mon]);
        all_info[loc][5][strlen(months[time->tm_mon])] = '\0';


        all_info[loc][6] = malloc(3);
        sprintf(all_info[loc][6], "%d", time->tm_mday);
        all_info[loc][6][2] = '\0';


        all_info[loc][7] = malloc(6);

        int hour = time->tm_hour;
        int min = time->tm_min;
        sprintf(all_info[loc][7], "%02i:%02i", hour, min);
        all_info[loc][7][5] = '\0';

        //File name
        all_info[loc][8] = malloc(strlen(dirp->d_name)+1);
        strcpy(all_info[loc][8], dirp->d_name);
        all_info[loc][8][strlen(dirp->d_name)] = '\0';

        loc++;
      }


      //Prints out the total blocks
      printf("total %d\n", total_blocks % 2 ? (total_blocks/2) + 1 : total_blocks/2);

      //Formats the information the same way the ls -l command does
      int prev = -1;
      int outdex = 0;

      //Used to print in alphabetical order
      for(int l = 0; l < dir_count; l++) {
        if((strcasecmp(all_info[outdex][8], all_info[l][8]) < 0)) {
          outdex = l;
        }
      }
      
      for(int i = 0; i < dir_count; i++) {
        //Used to print in alphabetical order
        int index = outdex;
        for(int l = 0; l < dir_count; l++) {
          if(((prev < 0 || strcasecmp(all_info[prev][8], all_info[l][8]) < 0) && strcasecmp(all_info[index][8], all_info[l][8]) > 0)) {
            index = l;
          }
        }
        prev = index;

        for(int j = 0; j < 9; j++) {
          //Finds the max length of the given column, then adds spaces to each print so that it will be formatted correctly
          int max_len = 0;
          for(int k = 0; k < dir_count; k++) {
            max_len = strlen(all_info[k][j]) > max_len ? strlen(all_info[k][j]) : max_len;
          }
          if(j) printf(" ");

          if(j != 8) {
            while(max_len - strlen(all_info[index][j])) {
              printf(" ");
              max_len--;
            }
          }

          printf("%s", all_info[index][j]);
          
        }
        printf("\n");
      }

      //Free the char*** and all of its elements
      for(int i = 0; i < dir_count; i++) {
        for(int j = 0; j < 9; j++) {
          free(all_info[i][j]);
        }
        free(all_info[i]);
      }

      free(all_info);

      if(closedir(dir) < 0) {
          perror("Error closing file");
      }
    }
  }
}

//cp: checks for the given files' existence, then opens the files, reads the input file, then writes to the output file
static void cp(char** args, int argcp) {
  if(argcp > 3)
    printf("Too many arguments\n");
  else if(argcp < 3)
    printf("Too few arguments\n");

  else {
    if (access(args[1], F_OK))
      printf("Cannot find input file\n");

    //Uses fopen in order to get files in any directory, checking for errors at each point possible
    else {

      //Check if given directory exists
      int dir_exists = 1;
      char arg_to_check[strlen(args[2])];
      int end = strlen(args[2]) - 1;
      while(end && args[2][end] != '/') {
        end--;
      }
      if(end) {
        strncpy(arg_to_check, args[2], end);
        arg_to_check[end] = '\0';
        if(access(arg_to_check, F_OK)) {
          printf("Directory does not exist\n");
          dir_exists = 0;
        }
      }

      if(dir_exists) {
        FILE* file_in;
        FILE* file_out;
        size_t num_bytes = 1;
        char buf[4096];

        if((file_in = fopen(args[1], "r")) == NULL) {
          perror("Opening input file error");
        }

        if((file_out = fopen(args[2], "w")) == NULL) {
          perror("Opening output file error");
        }

        while((num_bytes = fread(buf, sizeof(char), sizeof(buf), file_in)) > 0) {
          fwrite(buf, sizeof(char), num_bytes, file_out);
          if(ferror(file_out)) {
            perror("Writing error");
          }
        }
        if(ferror(file_in)) {
            perror("Reading error");
        }
        
        fclose(file_in);
        fclose(file_out);
      }
    }
  }
}

//touch: opens the given files, creating them id necessary, then updates the access times with futimens()
static void touch(char** args, int argcp) {
  if (argcp == 1)
    printf("Missing file operand\n");

  else {
    for(int i = 1; i < argcp; i++) {
      int fd;
      if((fd = open(args[i], O_CREAT, 0644)) < 0) {
        perror("Opening error");
      }

      if(futimens(fd, NULL) < 0) {
        perror("Error updating timestamp");
      }
      close(fd);
    }
  }
}
