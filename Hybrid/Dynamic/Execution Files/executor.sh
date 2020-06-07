#!/bin/bash

mpicc mandelbrot-hybrid-dynamic.c -fopenmp -lm -o mandelbrot-hybrid-dynamic

cat names.txt | while read line 
do
   qsub ${line}.sh
done