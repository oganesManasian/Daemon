#!/bin/bash

filename1='test_file1.txt'
filename2='test_file2.txt'
dir1="test_dir1"

# read dirs from config file
while IFS='' read -r path
do
  if [ "$path" = "" ]
  then
    break
  fi

  # do changes in path
  touch "$path/$filename1"
  touch "$path/$filename2"
  chmod +x "$path/$filenam2"
  mkdir "$path/$dir1"
  rm "$path/$filename1"

done < config.txt
