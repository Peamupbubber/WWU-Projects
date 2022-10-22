#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>
#include <stdlib.h>

void encrypt(int fd_out, char* buf, int number_bytes);
void decrypt(int fd_out, char* buf, int number_bytes);

int main(int argc, char** argv)
{
    //Ensure correct number of inputs are given
    if(argc != 3) {
        printf("Incorrent Usage: ./filesec -e|-d [filename]\n");
        return -1;
    }

    /* Sets up procedure to get the time that the program takes to run, used a source to learn how to do this:
     * https://linuxhint.com/gettimeofday_c_language/
     */
    struct timeval current_time;
    gettimeofday(&current_time, NULL);
    long int current_sec_1 = current_time.tv_sec;
    long int current_usec_1 = current_time.tv_usec;

    /* You may assume that the length of the output file name will not exceed 128 characters.
     *
     * Creates the output file name, used some sources to learn how to do this:
     * https://www.delftstack.com/howto/c/get-c-substring/
     * https://www.geeksforgeeks.org/how-to-append-a-character-to-a-string-in-c/
     */
    char output_file_name[128];
    int length_to_copy = strlen(argv[2]) - 4;
    
    strncpy(output_file_name, argv[2], length_to_copy);
    output_file_name[length_to_copy] = '\0';
    if(!strcmp(argv[1], "-e")) {
        strncat(output_file_name, "_enc.txt", 8);
    }
    else if(!strcmp(argv[1], "-d")) {
        strncat(output_file_name, "_dec.txt", 8);
    }
    else {
        printf("Incorrent Usage: ./filesec -e|-d [filename]\n");
        return -1;
    }

    //Opens the input and output files
    int fd_in;
    int fd_out;
    int number_bytes;
    char buf[4096];
    int calls = 0;

    if((fd_in = open(argv[2], O_RDONLY)) < 0) {
        perror("Opening error");
        exit(1);
    }

    if((fd_out = open(output_file_name, O_WRONLY | O_CREAT, 0644)) < 0) {
        perror("Opening error");
        exit(2);
    }

    //Undergoes the read / write process as long as there is text read
    while((number_bytes = read(fd_in, buf, sizeof(buf))) > 0) {
        if(number_bytes < 0) {
            perror("Reading error");
            exit(3);
        }

        //Call the requested function based on the tag
        if(!strcmp(argv[1], "-e")) {
            encrypt(fd_out, buf, number_bytes);
        }
        else {
            decrypt(fd_out, buf, number_bytes);
        }

        calls++;
    }

    close(fd_in);
    close(fd_out);

    //Outputs the amount of time the program took, accounting for errors in finding the difference in time. Also displays number of read / write calls
    gettimeofday(&current_time, NULL);
    long int seconds = current_time.tv_sec - current_sec_1;
    long int microseconds = current_time.tv_usec - current_usec_1;
    if(microseconds < 0) {
        seconds--;
        microseconds += 1000000;
    }
    printf("\nTime spent: %ld seconds and %ld microseconds\n", seconds, microseconds);
    printf("Number of times read() and write() were called: %d\n\n", calls);
    return 0;
}

//Encrypts the text in the given file by adding 100 to the ascii value of each character and writes to the output file
void encrypt(int fd_out, char* buf, int number_bytes) {
    for(int i = 0; i < number_bytes; i++) {
        buf[i] += 100;
    }
    
    if(write(fd_out, buf, number_bytes) < 0) {
        perror("Writing error");
        exit(4);
    }
}

//Decrypts the text in the given file by subtracting 100 to the ascii value of each character and writes to the output file
void decrypt(int fd_out, char* buf, int number_bytes) {
    for(int i = 0; i < number_bytes; i++) {
        buf[i] -= 100;
    }

    if(write(fd_out, buf, number_bytes) < 0) {
        perror("Writing error");
        exit(5);
    }
}