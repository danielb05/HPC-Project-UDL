//
//  mandelbrot.c
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

/*---- Generating Image Output ---------------------------------------------------------------*/

void color(int red, int green, int blue)
{
    fputc((char)red, stdout);
    fputc((char)green, stdout);
    fputc((char)blue, stdout);
}

int main(int argc, char *argv[])
{

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

    // Height x Width of the generated Image
    int w = 600, h = 400, x, y;

    // after how many iterations the function should stop
    int maxIterations = 10000;                

    // real and imaginary part of the pixel p
    double pr, pi;    

    // real and imaginary parts of new and old z
    double newRe, newIm, oldRe, oldIm;        
    
    // zoom and position
    double zoom = 1, moveX = -0.5, moveY = 0; 
    
    // colors [R, G ,B]
    typedef unsigned char pixel_t[3]; 

    // Allocating space for all the pixels
    pixel_t *pixels = malloc(sizeof(pixel_t)*h*w);

    // variables used to calculate execution time
    double time_spent, begin, end;
    

    /*---- Printing Execution Details --------------------------------------------------------*/

    printf("P6\n# CREATOR: Eric R. Weeks / mandel program\n");
    printf("%d %d\n255\n", w, h);


    /*---- Data ------------------------------------------------------------------------------*/
    
    // start counting execution time
    begin = omp_get_wtime();

    // Beginning of the OMP parallel section
    #pragma omp parallel shared(zoom, moveX, moveY, pixels) private(y, x, pr, pi, newRe, newIm, oldRe, oldIm)
    {
        // determining the scheduling type of the loop - dynamic
        // OpenMP divides the iterations into chunks of default size
        #pragma omp for schedule(dynamic)

        //loop through every pixel
        for (y = 0; y < h; y++)
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

                // color(i % 256, 255, 255 * (i < maxIterations));
                if (i == maxIterations)
                {
                    //color(0, 0, 0); // black
                    pixels[y * w + x][0] = 0;
                    pixels[y * w + x][1] = 0;
                    pixels[y * w + x][2] = 0;
                }

                else
                {
                    double z = sqrt(newRe * newRe + newIm * newIm);
                    int brightness = 256 * log2(1.75 + i - log2(log2(z))) / log2((double)maxIterations);
                    //color(brightness, brightness, 255);
                    pixels[y * w + x][0] = brightness;
                    pixels[y * w + x][1] = brightness;
                    pixels[y * w + x][2] = 255;
                }
            }
    }
    // End of the OMP parallel section

    // stop counting execution time 
    end = omp_get_wtime();


    /*---- Results ------------------------------------------------------------------------------*/

    // sends colors of each pixel to be printed in the output image
    for (y = 0; y < h; y++){
        for (x = 0; x < w; x++)
        {
            color(pixels[y * w + x][0], pixels[y * w + x][1], pixels[y * w + x][2]);
        }
    }

    // calculates time spent
    time_spent = (end - begin);

    // prints Elapsed time
    fprintf(stderr, "Elapsed time: %.4lf seconds.\n", time_spent);

    // deallocates the memory previously allocated
    free(pixels);

    // ends the program
    return 0;
}
