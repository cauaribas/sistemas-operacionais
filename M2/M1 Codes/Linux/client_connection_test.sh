#!/bin/bash

NUM_CLIENTS=100
WORD="palavra_teste"  # Defina a palavra que você quer enviar

# Medir o tempo de execução das conexões
  for ((i = 1; i <= NUM_CLIENTS; i++))
  do
    echo "$WORD" | ./string_consumer & 
    echo $i | ./number_consumer &
  done
  wait  # Espera que todos os clientes finalizem
