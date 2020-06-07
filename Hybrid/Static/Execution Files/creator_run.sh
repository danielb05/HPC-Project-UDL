#!/bin/bash

old=1-1
oldFile=${old}.sh

cat names.txt | while read line 
do
   sed "s/${old}/${line}/g" ${oldFile} > ${line}.sh
   old=${line}
   oldFile=${line}.sh
done