#!/bin/bash

cat names.txt | while read line 
do
   gcc -lm -o mandelbrot-OMP-${line} -fopenmp mandelbrot-OMP-${line}.c
   qsub ${line}.sh
done
