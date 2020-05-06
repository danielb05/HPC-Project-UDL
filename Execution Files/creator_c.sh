#!/bin/bash

old=1-1
oldFile=mandelbrot-OMP-${old}.c

cat c_names.txt | while read line 
do
   cp ${oldFile} mandelbrot-OMP-${line}.c
   old=${line}
   oldFile=mandelbrot-OMP-${line}.c
done
