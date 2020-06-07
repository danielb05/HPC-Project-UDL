//
//  mandelbrot-hybrid-dynamic.c
//
//
//  The Mandelbrot calculation is to iterate the equation
//  z = z*z + c, where z and c are complex numbers, z is initially
//  zero, and c is the coordinate of the point being tested. If
//  the magnitude of z remains less than 2 for ever, then the point
//  c is in the Mandelbrot set. In this code We write out the number of iterations
//  before the magnitude of z exceeds 2, or UCHAR_MAX, whichever is
//  smaller.//
//
//

#include <stdio.h>

#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <omp.h>
#include <mpi.h>

/*---- Declarations -------------------------------------------------------------------------
*   Height h, Width d, and Number of Iterations maxIterations
*   vary according with pre-determined combinations
* 
*   Possible Height x Width combinations:
*   - 600x400
*   - 3.000x2.000
*   - 6.000x4.000
*   - 30.000x20.000
* 
*   Possible Numbers of Iterations
*   - 10.000
*   - 100.000
*   - 1.000.000
* 
*   Note:
*   Each iteration, it calculates: newz = oldz*oldz + p, where p is the current pixel, and oldz stars at the origin
*------------------------------------------------------------------------------------------*/

/*---- Global Variables ---------------------------------------------------------------*/

// Height x Width of the generated Image
int w, h;

// Image size
int imageSize = 0;

// Determines after how many iterations the function should stop
int maxIterations = 10000;

// Used to indicate the number of threads used on the execution of the code
int numThreads = 0;

// Number of fragments in which the image will be splitted.
int splits = 1;

// Structure with 3 values, corresponding to Red, Green, and Blue
struct rgb
{
    int red, green, blue;
};


/*---- Declaring Functions ---------------------------------------------------------------*/

// Initializes the pixels array to avoid possible problems with left over values
void fillPixels(struct rgb *pixels);

// Joins the calculated Chunk of pixels to the complete Pixels array
void joinPixels(struct rgb *pixels, struct rgb *tempPixels, int initialPos, int finalPos);

// Iterates through the calculated pixels sending them to be printed to the output image
void printPixels(struct rgb *pixels);

// Prints the received RGB colors to the output image
void color(int red, int green, int blue);

// Calculates and prints execution time results and parameters
void getResults(double begin, double end, double end2, int size);


/*---- MAIN ---------------------------------------------------------------*/

int main(int argc, char *argv[])
{
    /*---- Getting User Inputs -----------------------------------------------------------*/

    // [1] = Image Width
    // [2] = Image Height
    // [3] = Maximum number of iterations
    // [4] = Number of threads of the execution
    // [5] = Number of Image splits
    if (argc >= 4)
    {
        w = atoi(argv[1]);
        h = atoi(argv[2]);
        maxIterations = atoi(argv[3]);
        numThreads = atoi(argv[4]);
        splits = atoi(argv[5]);
    }

    // Calculates the Image Size
    imageSize = w * h;


    /*---- Variables ---------------------------------------------------------------*/

    // Allocating space for all the pixels
    struct rgb *pixels = malloc(sizeof(struct rgb) * imageSize);

    // variables used to calculate execution time
    double begin, end, end2;

    // variables used by MPI
    int rank, size;

    // variables used on the master-worker process
    int nworkers, aux, aux2;

    // Value that represents the status of the received message.
    MPI_Status status;

    /*---- MPI --------------------------------------------------------*/

    // Starts MPI and returns an error If something wrong happens
    if (MPI_Init(&argc, &argv) != MPI_SUCCESS)
    {
        fprintf(stderr, "Error initilazing MPI\n");
        return 100;
    }

    // Get current process id
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // Get number of processes
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Defining the number of Workers
    nworkers = size - 1;

    // Determining the number of lines to bee calculated in each fragment
    int fragmentHeight = h / splits;


    /*---- Creating MPI_RGB struct ------------------------------------------------------------------------------*/

    MPI_Datatype MPI_RGB;

    MPI_Datatype type[3] = {MPI_INT, MPI_INT, MPI_INT};

    MPI_Type_commit(type);

    int blocklen[3] = {1, 1, 1};

    MPI_Aint displacement[3] = {0, sizeof(int), sizeof(int) * 2};

    MPI_Type_create_struct(3, blocklen, displacement, type, &MPI_RGB);
    
    MPI_Type_commit(&MPI_RGB);


    /*---- Executing ------------------------------------------------------------------------------*/

    /*---- Master --------*/
    if (rank == 0)
    {

        // Initializing  final image pixels array 
        fillPixels(pixels);

        // start counting execution time
        begin = MPI_Wtime();

        /*---- Printing Execution Details --------------------------------------------------------*/

        printf("P6\n# Original Code CREATOR: Eric R. Weeks / mandel program - Changes by: Daniel V. Cordeiro & Rafael C. Pereira\n");
        printf("%d %d\n255\n", w, h);


        /*---- Managing MPI Message Exchange and Image Calculation --------------------------------------------------------*/

        // Counts how many messages (fragments) have already been received
        int rcvdMsgs = 0;

        // Sends the firts fragment for each "worker" MPI process
        for (aux2 = 0; aux2 < nworkers; aux2++)
        {
            MPI_Send(&aux2, 1, MPI_INT, aux2 + 1, rank, MPI_COMM_WORLD);
        }

        // Receives the messages (fragments) have already been calculated
        for (aux = 0; aux < splits; aux++)
        {
            // Allocating space for current chunk of pixels
            struct rgb *rcvdPixels = malloc(sizeof(struct rgb) * imageSize);

            // Receiving calculated fragment of mandelbtrot
            MPI_Recv(rcvdPixels, imageSize, MPI_RGB, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

            // From which process this message comes from
            int source = status.MPI_SOURCE;

            // Fragment number
            int tag = status.MPI_TAG;

            // Calculating the initial position of that fragment
            int initialPos = tag * fragmentHeight;

            // Calculating the final position of that fragment
            int finalPos = fragmentHeight * (tag + 1);

            // Joins the received fragment with the final complete image
            joinPixels(pixels, rcvdPixels, initialPos, finalPos);

            // Deallocates the memory previously allocated
            free(rcvdPixels);

            // If this is not the last fragment, increase the fragment counter
            if ((aux + nworkers) < splits)
            {
                rcvdMsgs = aux + nworkers;
            }

            // Otherwise sets a signal to the worker to finish working
            else
            {
                rcvdMsgs = -1;
            }

            // Sends a message to the worker, either with the number of the next fragment to be calculated or to stop working
            MPI_Send(&rcvdMsgs, 1, MPI_INT, source, source, MPI_COMM_WORLD);
        }

        // Stops counting execution time 
        end = MPI_Wtime();

        // Print calculated image
        printPixels(pixels);

        // Stops counting execution time - taking into account printing time
        end2 = MPI_Wtime();

        /*---- Results ------------------------------------------------------------------------------*/

        // Calculates and prints execution data
        getResults(begin, end, end2, size);

        // Deallocates the memory previously allocated
        free(pixels);
    }

    /*---- Worker --------*/

    else
    {
        /*---- Execution ------------------------------------------------------------------------------*/
        
        // Execute forever (or until receive a comand to stop)
        while (1)
        {
            // Number of fragment
            int pos = 0;

            // Receiving information about the fragment to be calculated
            MPI_Recv(&pos, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

            // If it is a "-1" it is a message to stop working
            if (pos == -1)
            {
                break;
            }

            // Calculate the Mandelbrot fragment
            else
            {
                // variables used for iteration
                int x, y;

                // variable to hold 1 calculated pixel colors [R, G ,B]
                struct rgb pixel;

                // initial position of that fragment
                int initialPos = pos * fragmentHeight;

                // final position of that fragment
                int finalPos = fragmentHeight * (pos+1);

                // real and imaginary part of the pixel p
                double pr, pi;

                // real and imaginary parts of new and old z
                double newRe, newIm, oldRe, oldIm;

                // zoom and position
                double zoom = 1, moveX = -0.5, moveY = 0;

                // Allocating space for the pixels in this rank
                struct rgb *localPixels = malloc(sizeof(struct rgb) * imageSize);

                // Beginning of the OMP parallel section
                #pragma omp parallel shared(zoom, moveX, moveY, localPixels, initialPos) private(y, x, pr, pi, newRe, newIm, oldRe, oldIm, pixel)
                {
                    // Determining the scheduling type of the loop - dynamic
                    // OpenMP divides the iterations into chunks of default size
                    #pragma omp for schedule(dynamic)
                    
                    //loop through every pixel
                    for (y = initialPos; y < finalPos; y++)
                    {
                        for (x = 0; x < w; x++)
                        {
                            // calculate the initial real and imaginary part of z, based on the pixel location and zoom and position values
                            pr = 1.5 * (x - w / 2) / (0.5 * zoom * w) + moveX;
                            pi = (y - h / 2) / (0.5 * zoom * h) + moveY;
                            newRe = newIm = oldRe = oldIm = 0; //these should start at 0,0

                            // "i" will represent the number of iterations
                            int i;

                            // start the iteration process
                            for (i = 0; i < maxIterations; i++)
                            {
                                // remember value of previous iteration
                                oldRe = newRe;
                                oldIm = newIm;
                                //the actual iteration, the real and imaginary part are calculated
                                newRe = oldRe * oldRe - oldIm * oldIm + pr;
                                newIm = 2 * oldRe * oldIm + pi;

                                //if the point is outside the circle with radius 2: stop
                                if ((newRe * newRe + newIm * newIm) > 4)
                                    break;
                            }

                            // Calculates position of the current calculated pixel
                            int posLocal = (y - initialPos) * w + x;

                            // color(i % 256, 255, 255 * (i < maxIterations));
                            if (i == maxIterations)
                            {
                                //color(0, 0, 0); // black
                                pixel.red = 0;
                                pixel.green = 0;
                                pixel.blue = 0;
                            }

                            else
                            {
                                double z = sqrt(newRe * newRe + newIm * newIm);
                                int brightness = 256 * log2(1.75 + i - log2(log2(z))) / log2((double)maxIterations);
                                
                                //color(brightness, brightness, 255);
                                pixel.red = brightness;
                                pixel.green = brightness;
                                pixel.blue = 255;
                            }

                            // Putting the calculated pixel in the fragment array
                            localPixels[posLocal] = pixel;
                        }
                    }
                    // End of the OMP parallel section
                }

                // Sends back to the master process the calculated fragment
                MPI_Send(localPixels, imageSize, MPI_RGB, 0, pos, MPI_COMM_WORLD);
            }
        }
    }

    // Terminates MPI execution environment
    MPI_Finalize();

    // ends the program
    return 0;
}


/*---- Auxiliar Functions ---------------------------------------------------------------*/

// Initializes the pixels array to avoid possible problems with left over values
void fillPixels(struct rgb *pixels)
{
    // creating and initializing a black pixel variable
    struct rgb pixel;
    pixel.red = 0;
    pixel.green = 0;
    pixel.blue = 0;

    // used to iterate over the pixels array
    int p;

    for (p = 0; p < imageSize; p++)
    {
        pixels[p] = pixel;
    }
}

// Joins the calculated Chunk of pixels to the complete Pixels array
void joinPixels(struct rgb *pixels, struct rgb *tempPixels, int initialPos, int finalPos)
{
    // calculating initial and final position of the received fragment
    initialPos = initialPos * w;
    finalPos = finalPos * w;

    // position in the final image array
    int p;

    //position in the received fragment array
    int pos = 0;

    for (p = initialPos; p < finalPos; p++, pos++)
    {
        pixels[p] = tempPixels[pos];
    }
}

// Iterates through the calculated pixels sending them to be printed to the output image
void printPixels(struct rgb *pixels)
{
    // variables used to iterate over the pixels array
    int py, px;

    // sends colors of each pixel to be printed in the output image
    for (py = 0; py < h; py++)
    {
        for (px = 0; px < w; px++)
        {
            color((pixels[py * w + px]).red, (pixels[py * w + px]).green, (pixels[py * w + px]).blue);
        }
    }
}

// Prints the received RGB colors to the output image
void color(int red, int green, int blue)
{
    fputc((char)red, stdout);
    fputc((char)green, stdout);
    fputc((char)blue, stdout);
}

// Calculates and prints execution time results and parameters
void getResults(double begin, double end, double end2, int size)
{
    // calculates time spent without printing
    double time_spent = (end - begin);

    // calculates time spent with printing
    double time_spent2 = (end2 - begin);

    // prints Execution Details
    fprintf(stderr, "W: %d, H: %d, Iterations: %d Processes:%i Threads: %i Splits: %i\n", w, h, maxIterations, size, numThreads, splits);

    // prints Elapsed times without printing
    fprintf(stderr, "\nElapsed time: %.4lf seconds.\n", time_spent);

    // prints Elapsed times with printing
    fprintf(stderr, "\nElapsed time with printing: %.4lf seconds.\n", time_spent2);
}
