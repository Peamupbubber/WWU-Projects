#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>

#define LAPLACIAN_THREADS 4     //change the number of threads as you run your concurrency experiment

/* Laplacian filter is 3 by 3 */
#define FILTER_WIDTH  3
#define FILTER_HEIGHT 3

#define RGB_COMPONENT_COLOR 255

typedef struct {
      unsigned char r, g, b;
} PPMPixel;

struct parameter {
    PPMPixel *image;         //original image pixel data
    PPMPixel *result;        //filtered image pixel data
    unsigned long int w;     //width of image
    unsigned long int h;     //height of image
    unsigned long int start; //starting point of work
    unsigned long int size;  //equal share of work (almost equal if odd)
};

struct file_name_args {
    char *input_file_name;      //e.g., file1.ppm
    char output_file_name[20];  //will take the form laplaciani.ppm, e.g., laplacian1.ppm
};

int check_if_ppm(char* name); //method for checking if a file is of type .ppm

double total_elapsed_time = 0;

pthread_barrier_t file_barrier;
pthread_mutex_t mutex;

/*This is the thread function. It will compute the new values for the region of image specified in params (start to start+size) using convolution.
    For each pixel in the input image, the filter is conceptually placed on top of the image with its origin lying on that pixel.
    The  values  of  each  input  image  pixel  under  the  mask  are  multiplied  by the corresponding filter values.
    Truncate values smaller than zero to zero and larger than 255 to 255.
    The results are summed together to yield a single output value that is placed in the output image at the location of the pixel being processed on the input.
 */
void *compute_laplacian_threadfn(void *params)
{
    int laplacian[FILTER_WIDTH][FILTER_HEIGHT] =
    {
        {-1, -1, -1},
        {-1,  8, -1},
        {-1, -1, -1}
    };

    int red, green, blue;

    struct parameter *param = (struct parameter *) params;

    //Algorithm for applying the laplacian filter
    for(long unsigned int w = 0; w < param->w; w++) {
        for(long unsigned int h = param->start; h < (param->start + param->size); h++) {
            red = 0;
            green = 0;
            blue = 0;

            for(int i = 0; i < FILTER_WIDTH; i++) {
                for(int j = 0; j < FILTER_HEIGHT; j++) {
                    int x_cord = (w - FILTER_WIDTH/2 + i + param->w)%param->w;
                    int y_cord = (h - FILTER_HEIGHT/2 + j + param->h)%param->h;

                    red += param->image[y_cord * param->w + x_cord].r * laplacian[i][j];
                    green += param->image[y_cord * param->w + x_cord].g * laplacian[i][j];
                    blue += param->image[y_cord * param->w + x_cord].b * laplacian[i][j];
                }
            }

            //Keep the colors within the bounds of (0, 255)
            if(red > 255)
                red = 255;
            else if(red < 0)
                red = 0;

            if(green > 255)
                green = 255;
            else if(green < 0)
                green = 0;

            if(blue > 255)
                blue = 255;
            else if(blue < 0)
                blue = 0;

            param->result[h * param->w + w].r = red;
            param->result[h * param->w + w].g = green;
            param->result[h * param->w + w].b = blue;
        }
    }

    return NULL;
}

/* Apply the Laplacian filter to an image using threads.
 Each thread shall do an equal share of the work, i.e. work=height/number of threads. If the size is not even, the last thread shall take the rest of the work.
 Compute the elapsed time and store it in *elapsedTime (Read about gettimeofday).
 Return: result (filtered image)
 */
PPMPixel *apply_filters(PPMPixel *image, unsigned long w, unsigned long h, double *elapsedTime) {
    //Begin gathering the time
    struct timeval start_time, end_time;
    gettimeofday(&start_time, NULL);
    
    long unsigned int start = 0;
    PPMPixel* result = malloc(w * h * sizeof(PPMPixel));

    pthread_t threads[LAPLACIAN_THREADS];

    struct parameter** params = (struct parameter **)calloc(sizeof(struct parameter*), LAPLACIAN_THREADS);
    int index = 0;
    
    //Create the appropriete amount of threads
    for(int i = 0; i < LAPLACIAN_THREADS; i++) {
        //Initialize the parameters
        params[index] = (struct parameter *)calloc(sizeof(struct parameter),1);
        params[index]->image = image;
        params[index]->result = result;
        params[index]->w = w;
        params[index]->h = h;
        params[index]->start = start;
        params[index]->size = h/LAPLACIAN_THREADS;
        
        if(i == LAPLACIAN_THREADS - 1) {
            params[index]->size += h%LAPLACIAN_THREADS;
        }

        if(pthread_create(&threads[i], NULL, compute_laplacian_threadfn, params[index])) {
            perror("Error creating thread");
            exit(EXIT_FAILURE);
        }
        start += params[index]->size;
        index++;
    }
    
    //Join each thread and free the params
    for(int i = 0; i < LAPLACIAN_THREADS; i++) {
        pthread_join(threads[i], NULL);
        free(params[i]);
    }
    free(params);

    //Compute elapsed time
    gettimeofday(&end_time, NULL);
    *elapsedTime += (double)(end_time.tv_sec - start_time.tv_sec) + (double)(end_time.tv_usec - start_time.tv_usec) / 1000000.0;

    return result;
}

/*Create a new P6 file to save the filtered image in. Write the header block
 e.g. P6
      Width Height
      Max color value
 then write the image data.
 The name of the new file shall be "filename" (the second argument).
 */
void write_image(PPMPixel *image, char *filename, unsigned long int width, unsigned long int height)
{
    FILE* file_out;

    if((file_out = fopen(filename, "wb")) == NULL) {
        perror("Opening input file error");
        exit(EXIT_FAILURE);
    }

    //Place the header info
    char header_info[32];
    if (sprintf(header_info, "P6\n%ld %ld\n%d\n", width, height, RGB_COMPONENT_COLOR) < 0) {
        perror("Error creating new header information");
        exit(EXIT_FAILURE);
    }
    fputs(header_info, file_out);
    if(ferror(file_out)) {
        perror("Error writing file");
        exit(EXIT_FAILURE);
    }

    fwrite(image, sizeof(PPMPixel) * width, height, file_out);
    if(ferror(file_out)) {
        perror("Error writing file");
        exit(EXIT_FAILURE);
    }

    fclose(file_out);
}

/* Open the filename image for reading, and parse it.
    Example of a ppm header:    //http://netpbm.sourceforge.net/doc/ppm.html
    P6                  -- image format
    # comment           -- comment lines begin with
    ## another comment  -- any number of comment lines
    200 300             -- image width & height
    255                 -- max color value
 
 Check if the image format is P6. If not, print invalid format error message.
 If there are comments in the file, skip them. You may assume that comments exist only in the header block.
 Read the image size information and store them in width and height.
 Check the rgb component, if not 255, display error message.
 Return: pointer to PPMPixel that has the pixel data of the input image (filename).The pixel data is stored in scanline order from left to right (up to bottom) in 3-byte chunks (r g b values for each pixel) encoded as binary numbers.
 */
PPMPixel *read_image(const char *filename, unsigned long int *width, unsigned long int *height)
{
    FILE* file_in;

    if((file_in = fopen(filename, "rb")) == NULL) {
        perror("Opening input file error");
    }

    //Make sure the file is of type P6
    if(!(fgetc(file_in) == 'P' && fgetc(file_in) == '6')) {
        if(ferror(file_in)) {
            perror("Error reading file");
            fclose(file_in);
            return NULL;
        }
        printf("%s: file unsupported, image format must be P6\n", filename);
        fclose(file_in);
        return NULL;
    }
    while(fgetc(file_in) != '\n') {
        if(ferror(file_in)) {
            perror("Error reading file");
            fclose(file_in);
            return NULL;
        }
    }
    
    //Skip over comments
    char c = fgetc(file_in);
    while(c == '#') {
        if(ferror(file_in)) {
            perror("Error reading file");
            fclose(file_in);
            return NULL;
        }
        while((c = fgetc(file_in)) != '\n') {
            if(ferror(file_in)) {
                perror("Error reading file");
                fclose(file_in);
                return NULL;
            }
        }
    }

    //Set width and height
    fscanf(file_in, "%ld %ld" , width, height);
    if(ferror(file_in)) {
        perror("Error reading file");
        fclose(file_in);
        return NULL;
    }

    //Ensure correct color value
    int rgb;
    fscanf(file_in, "%d", &rgb);
    if(rgb != RGB_COMPONENT_COLOR) {
        printf("%s: file unsupported, max color value must be %d\n", filename, RGB_COMPONENT_COLOR);
        fclose(file_in);
        return NULL;
    }
    while(fgetc(file_in) != '\n');

    //Reads in the pixel data
    PPMPixel *img = malloc(*width * *height * sizeof(PPMPixel));
    int index = 0;

    while(index < *width * *height) {
        fread(&img[index], sizeof(PPMPixel), 1, file_in);
        if(ferror(file_in)) {
            perror("Error reading file");
            free(img);
            fclose(file_in);
            return NULL;
        }
        index++;
    }
    if(ferror(file_in)) {
        perror("Error reading file");
        free(img);
        fclose(file_in);
        return NULL;
    }
    
    fclose(file_in);
    return img;
}

/* The thread function that manages an image file. 
 Read an image file that is passed as an argument at runtime. 
 Apply the Laplacian filter. 
 Save the result image in a file called laplaciani.ppm, where i is the image file order in the passed arguments.
 Example: the result image of the file passed third during the input shall be called "laplacian3.ppm".
*/
void *manage_image_file(void *args){
    unsigned long int width;
    unsigned long int height;

    struct file_name_args* filename = (struct file_name_args*)args;

    PPMPixel* image = read_image(filename->input_file_name, &width, &height);

    //Make sure image was read in properly
    if(image != NULL) {
        pthread_mutex_lock(&mutex);
        double elapsed_time = 0;
        PPMPixel* result = apply_filters(image, width, height, &elapsed_time);
        total_elapsed_time += elapsed_time;
        write_image(result, filename->output_file_name, width, height);

        printf("%s: Time consumed: %.4f\n", filename->input_file_name, elapsed_time);
        pthread_mutex_unlock(&mutex);

        free(result);
        free(image);
    }

    pthread_barrier_wait(&file_barrier);
    return NULL;
}

//Checks to see if the given strings' last 4 characters are .ppm
int check_if_ppm(char* name) {
   int len = strlen(name);
   return len >= 4  && name[len-4] == '.'
                    && name[len-3] == 'p'
                    && name[len-2] == 'p'
                    && name[len-1] == 'm';
}

/*The driver of the program. Check for the correct number of arguments. If wrong print the message: "Usage ./a.out filename[s]"
  It shall accept n filenames as arguments, separated by whitespace, e.g., ./a.out file1.ppm file2.ppm    file3.ppm
  It will create a thread for each input file to manage.
  It will print the total elapsed time in .4 precision seconds(e.g., 0.1234 s).
  The total elapsed time is the total time taken by all threads to compute the edge detection of all input images.
 */
int main(int argc, char *argv[])
{
    if(argc < 2) printf("Usage: ./edge_detector filename ...\n");

    else {
        //Find the amount of threads to create (valid and existing files)
        int threadCount = argc - 1;
        for(int i = 1; i < argc; i++) {
            if(!check_if_ppm(argv[i]) || access(argv[i], F_OK))
                threadCount--;
        }
        struct file_name_args* filenames = (struct file_name_args *)calloc(threadCount, sizeof(struct file_name_args));

        pthread_barrier_init(&file_barrier, NULL, threadCount);
        pthread_mutex_init(&mutex, NULL);

        //Create a thread for each valid and existing file
        pthread_t threads[threadCount];
        int threadIndex = 0;
        for(int i = 1; i < argc; i++) {
            if(check_if_ppm(argv[i])) {
                if(access(argv[i], F_OK))
                    printf("%s: Cannot find file\n", argv[i]);
                else {
                    //Create the input / output file names
                    filenames[threadIndex].input_file_name = malloc(strlen(argv[i])+1);
                    strcpy(filenames[threadIndex].input_file_name, argv[i]);
                    (filenames[threadIndex].input_file_name)[strlen(argv[i])] = '\0';

                    if(snprintf(filenames[threadIndex].output_file_name, 20, "laplacian%d.ppm", threadIndex+1) < 0) {
                        perror("Error creating output file name");
                        exit(EXIT_FAILURE);
                    }

                    if(pthread_create(&threads[threadIndex], NULL, manage_image_file, (void*)&filenames[threadIndex])) {
                        perror("Error creating thread");
                        exit(EXIT_FAILURE);
                    }
                    threadIndex++;
                }
            }
            else
                printf("%s: file unsupported\n", argv[i]);
        }

        //Join the threads and free the filenames
        for(int i = 0; i < threadCount; i++) {
            pthread_join(threads[i], NULL);
            free(filenames[i].input_file_name);
        }
        free(filenames);

        pthread_barrier_destroy(&file_barrier);
        pthread_mutex_destroy(&mutex);

        printf("Total elapsed time is %.4f\n", total_elapsed_time);
        printf("Image processing is complete.\n");
    }

    return EXIT_SUCCESS;
}
