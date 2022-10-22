// Includes
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
#include <string.h>
#include <pwd.h>
#include <dirent.h>
#include "builtin.h"
#include <grp.h>
#include <stdbool.h>
#include <time.h>
#include <sys/sysmacros.h>


// Default starting buffer size for strings of unknown length.
#define DEFAULT_BUFFER_SIZE (32)

// tail - Chunk size for the tail command's reading procedure.
#define TAIL_CHUNK_SIZE (32)

// tail - How many lines tail should print.
#define TAIL_NUMBER_LINES (10)

// stat - Permissions mask for stat to get relevant bits.
#define STAT_PERMISSION_MASK (0777)

// stat - Formatted date length.
#define STAT_DATE_LENGTH (30)

// stat - Formatted device length.
#define STAT_DEVICE_LENGTH (32)

// Numeric characters for parsing the exit code argument.
static const char* EXIT_NUMERIC_CHARACTERS = "0123456789";

// Format for dates printed by stat.
static const char* STAT_FORMAT_DATE = "%Y-%m-%d %H:%M:%S";

// Prototypes
static void cmd_exit(char** args, int argc);
static void cmd_cd(char** args, int argpcp);
static void cmd_pwd(char** args, int argc);
static void cmd_stat(char** args, int argc);
static void cmd_env(char** args, int argc);
static void cmd_tail(char** args, int argc);


/* Required - built_in 
 * built_in checks if the first passed argument is one of the built in commands.
 * If so, that command is executed and 0 is returned.
 * If not, 0 is returned.
 *
 * Arguments are validated here so there is no need to check them in the below functions.
 */
int built_in(char** args, int argc)
{
	// Check for invalid arguments.
	bool invalid_arguments = false;
	
	if(args != NULL)
	{
		for(unsigned i = 0; i < argc; i++)
		{
			invalid_arguments = (args[i] == NULL) || invalid_arguments;
		}
	}
	else
	{
		invalid_arguments = true;
	}
	
	if(invalid_arguments)
	{
		fprintf(stderr, "myshell: Malformed argument list fed to 'built_in'.\n");
		exit(-1);
	}
	
	
	// See if the command is built in. If it is, call it with the arguments passed from main.
	bool is_built_in = true;
	
	if(strncmp(args[0], "exit", 4) == 0)
	{
		cmd_exit(args, argc);
	}
	else if(strncmp(args[0], "cd", 2) == 0)
	{
		cmd_cd(args, argc);
	}
	else if(strncmp(args[0], "pwd", 3) == 0)
	{
		cmd_pwd(args, argc);
	}
	else if(strncmp(args[0], "stat", 4) == 0)
	{
		cmd_stat(args, argc);
	}
	else if(strncmp(args[0], "env", 3) == 0)
	{
		cmd_env(args, argc);
	}
	else if(strncmp(args[0], "tail", 4) == 0)
	{
		cmd_tail(args, argc);
	}
	else
	{
		is_built_in = false;
	}
	
	return is_built_in;
}

/* Required - cmd_exit
 * Exits with zero when passed no arguments, or if a code is passed,
 * it attempts to parse said string as a number. If this succeeds,
 * myshell is terminated with that exit code.
 */
static void cmd_exit(char** args, int argc)
{
	if(argc == 1)
	{
		exit(0);
	}
	else if(argc == 2)
	{
		// strspn(3) is used to assure that all characters in the argument are numeric.
		if(strspn(args[1], EXIT_NUMERIC_CHARACTERS) == strlen(args[1]))
		{
			exit(atoi(args[1]));
		}
		else
		{
			printf("exit: '%s' is not a valid exit code.\n", args[1]);
		}
	}
	else
	{
		printf("exit: Takes 0..1 arguments.\n");
	}
}

/* Required - cmd_pwd
 * Simply reads the current working directory from getcwd(3) and prints it.
 */
static void cmd_pwd(char** args, int argc)
{
	if(argc == 1)
	{
		size_t size = DEFAULT_BUFFER_SIZE;
		char* name_buffer = malloc(size);
		getcwd(name_buffer, size);
		
		// If the buffer is too small to hold the path string, retry with a larger buffer.
		while(!strcmp(name_buffer, ""))
		{
			size *= 2;
			name_buffer = realloc(name_buffer, size);
			getcwd(name_buffer, size);
		}
		
		printf("%s\n", name_buffer);
		
		free(name_buffer);
	}
	else
	{
		printf("pwd: Doesn't take any arguments.\n");
	}
}

/* Required - cmd_cd
 * Navigates the directory specified in the first argument.
 * If no arguments are passed, HOME is navigated to instead.
 */
static void cmd_cd(char** args, int argc)
{
	if(argc == 1)
	{
		char* directory = getenv("HOME");
		
		if(chdir(directory) < 0)
		{
			perror("cd");
		}
	}
	else if(argc == 2)
	{
		if(chdir(args[1]) < 0)
		{
			perror("cd");
		}
	}
	else
	{
		printf("cd: Takes 0..1 arguments.\n");
	}
}

/* Required - cmd_stat
 * Prints all info about a file from struct stat in a formatted fashion.
 */
static void cmd_stat(char** args, int argc)
{
	struct stat file_info;
	
	for(size_t i = 1; i < argc; i++)
	{
		// Get file information.
		int result = stat(args[i], &file_info);
		
		// Check for errors
		if(result < 0)
		{
			perror("stat");
			continue;
		}
		
		// Permission string - MUST be 11 characters.
		char permission_string[11];
		
		// Get file permissions and type.
		mode_t mode = file_info.st_mode;
		
		// Determine what type the file is, and what string to display accordingly.
		char* file_type = "unknown file";
		if(S_ISREG(mode))
		{
			permission_string[0] = '-';
			file_type = "regular file";
		}
		else if (S_ISDIR(mode))
		{
			permission_string[0] = 'd';
			file_type = "directory";
		}
		else if (S_ISLNK(mode))
		{
			permission_string[0] = 'l';
			file_type = "symbolic link";
		}
		else if (S_ISCHR(mode))
		{
			permission_string[0] = 'c';
			file_type = "character device";
		}
		else if (S_ISBLK(mode))
		{
			permission_string[0] = 'b';
			file_type = "block device";
		}
		else if (S_ISFIFO(mode))
		{
			permission_string[0] = 'p';
			file_type = "pipe";
		}
		else if (S_ISSOCK(mode))
		{
			permission_string[0] = 's';
			file_type = "local socket";
		}
		
		// Create the permission string.
		permission_string[1] = (mode & S_IRUSR ? 'r' : '-');
		permission_string[2] = (mode & S_IWUSR ? 'w' : '-');
		permission_string[3] = (mode & S_IXUSR ? 'x' : '-');
		permission_string[4] = (mode & S_IRGRP ? 'r' : '-');
		permission_string[5] = (mode & S_IWGRP ? 'w' : '-');
		permission_string[6] = (mode & S_IXGRP ? 'x' : '-');
		permission_string[7] = (mode & S_IROTH ? 'r' : '-');
		permission_string[8] = (mode & S_IWOTH ? 'w' : '-');
		permission_string[9] = (mode & S_IXOTH ? 'x' : '-');
		permission_string[10] = '\0';
		
		// Format the device id in both hexadecimal and decimal.
		char device_format[STAT_DEVICE_LENGTH];
		sprintf(device_format, "%lxh/%lud", file_info.st_dev, file_info.st_dev);
		
		// Buffer for the formatted dates.
		char date[STAT_DATE_LENGTH];
		
		// Print the general file statistics: size, blocks, inode, device id, links, etc.
		printf("  File: %s\n", args[i]);
		printf("  Size: %-16luBlocks: %-11luIO Block: %-7lu%s\n", file_info.st_size, file_info.st_blocks, file_info.st_blksize, file_type);
		printf("Device: %-16sInode: %-12luLinks: %-10lu\n", device_format,file_info.st_ino, file_info.st_nlink);
		
		// Get the current user's username and group name.
		char* username = getpwuid(file_info.st_uid)->pw_name;
		char* group_name = getgrgid(file_info.st_gid)->gr_name;
		printf(" Access: (%04o/%s)  Uid: (%5u/%8s)   Gid: (%5u/%8s)\n", mode & STAT_PERMISSION_MASK, permission_string, file_info.st_uid, username, file_info.st_gid, group_name);
		
		// Print the accessed, modified, and changed dates/times. 
		// Dates are formatted using the 'tm' struct and the 'strftime' functions.
		struct tm* time_info = localtime(&file_info.st_atim.tv_sec);
		int gmt_offset = time_info->tm_gmtoff;
		
		strftime(date, STAT_DATE_LENGTH, STAT_FORMAT_DATE, time_info);
		printf("Access: %s.%lu +%04i\n", date, file_info.st_atim.tv_nsec, gmt_offset);
		
		time_info = localtime(&file_info.st_mtim.tv_sec);
		strftime(date, STAT_DATE_LENGTH, STAT_FORMAT_DATE, time_info);
		printf("Modify: %s.%lu +%04i\n", date, file_info.st_mtim.tv_nsec, gmt_offset);
		
		time_info = localtime(&file_info.st_ctim.tv_sec);
		strftime(date, STAT_DATE_LENGTH, STAT_FORMAT_DATE, time_info);
		printf("Change: %s.%lu +%04i\n", date, file_info.st_ctim.tv_nsec, gmt_offset);
		
		endpwent();
	}
}

/* Required - cmd_env
 * Prints all environment variables.
 * If an argument in the form 'NAME=VALUE' is present, the environment 
 * variable 'NAME' is given the value 'VALUE' before printing
 */
static void cmd_env(char** args, int argc)
{
	// Get all environment variables.
	extern char** environ;
	
	if(argc < 3)
	{
		if(argc == 2 && strchr(args[1], '=') != NULL)
		{
			// Get the variable expression.
			char* expression = args[1];
			
			// Attempt to parse the name and value from the string.
			// The equal sign marks the end of the name and the start of the value.
			size_t equals_position = strcspn(expression, "=");
			
			char* variable_name = malloc(equals_position + 1);
			strncpy(variable_name, expression, equals_position);
			variable_name[equals_position] = '\0';
			
			size_t value_length = strlen(expression) - equals_position;
			char* value = malloc(value_length + 1);
			strncpy(value, &(expression[equals_position + 1]), equals_position);
			value[value_length] = '\0';
			
			int result = setenv(variable_name, value, 1);
			
			if(result < 0)
			{
				perror("env");
			}
			
			free(variable_name);
			free(value);
		}			
		else
		{
			printf("env: Malformed expression.\n");
		}

		for(char** pointer = environ; *pointer != NULL; pointer++)
		{
			printf("%s\n", *pointer);
		}
	}
	else
	{
		printf("env: Takes 0..1 arguments.\n");
	}
}

/* Required - cmd_tail
 * Prints the last ten lines of any amount of files. If multiple are listed,
 * headers are printed to differentiate between the output.
 */
static void cmd_tail(char** args, int argc)
{	
	for(unsigned file_index = 1; file_index < argc; file_index++)
	{
		// Get the name, open the file and make sure it exists.
		char* name = args[file_index];
		
		FILE* subject_file = fopen(name, "r");
		
		if(subject_file == NULL)
		{
			perror("tail");
			continue;
		}
		
		// Print the header if there are multiple files.
		if(argc > 2)
		{
			printf("==> %s <==\n", name);
		}
		
		// Get the file length
		fseek(subject_file, 0, SEEK_END);
		size_t file_end = ftell(subject_file);
		
		// Calculate how many iterations it will take to read through the file using TAIL_CHUNK_SIZE.
		// If there is a remainder (the length doesnt divide cleanly by TAIL_CHUNK_SIZE) an extra iteration is added to handle it.
		size_t remainder = file_end % TAIL_CHUNK_SIZE;
		bool uneven_division = (remainder > 1);
		size_t iterations = (file_end / TAIL_CHUNK_SIZE) + uneven_division;
		
		// Initialize loop variables/buffers.
		size_t newline_position = 0;
		size_t newlines_total = 0;
		size_t chunk_size = TAIL_CHUNK_SIZE;
		char chunk_buffer[chunk_size];
		
		// Read through the file backwards until the final newline is encountered.
		for(size_t i = 0; i < iterations; i++)
		{
			// If this is the uneven last iteration, change the chunk size so we don't read too much data.
			// Else, read the normal chunk size.
			if(i == iterations - 1 && uneven_division)
			{
				chunk_size = remainder;
				fseek(subject_file, 0, SEEK_SET);
			}
			else
			{
				fseek(subject_file, -TAIL_CHUNK_SIZE * (i + 1), SEEK_END);
			}
			
			// Read the data into the buffer.
			fread(chunk_buffer, 1, chunk_size, subject_file);
			
			// Iterate through the characters backwards.
			// This is so we encounter the final newline in the correct order.
			for(int j = TAIL_CHUNK_SIZE - 1; j >= 0; j--)
			{
				if(chunk_buffer[j] == '\n')
				{
					// Count newlines.
					++newlines_total;
					
					// If we have found the final newline, record its position and break out of the loop.
					if(newlines_total >= TAIL_NUMBER_LINES)
					{
						newline_position = ftell(subject_file) - chunk_size + j + 1;
						break;
					}
				}
			}
			
			// Break out again if the final newline was found.
			if(newlines_total >= TAIL_NUMBER_LINES)
			{
				break;
			}
		}
		
		// Use fseek(3) to read from the final newline's position until the end of file.
		size_t final_size = file_end - newline_position + 1;
		char output_buffer[final_size];
		
		fseek(subject_file, newline_position, SEEK_SET);
		
		fread(output_buffer, 1, final_size, subject_file);
		output_buffer[final_size - 1] = '\0';
		printf("%s\n", output_buffer);

		fclose(subject_file);
	}
	
	if(argc == 1)
	{
		printf("tail: Takes 1..* arguments.\n");
	}
}
