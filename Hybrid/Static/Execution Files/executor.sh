#!/bin/bash

mpicc mandelbrot-hybrid-static.c -fopenmp -lm -o mandelbrot-hybrid-static

cat names.txt | while read line 
do
   qsub ${line}.sh
done