#!/bin/bash

NUM_CLIENTS=100
WORD="palavra_teste" 

  for ((i = 1; i <= NUM_CLIENTS; i++))
  do
    echo "$WORD" | ./string_consumer & 
    echo $i | ./number_consumer &
  done
  wait  