#!/bin/bash

NUM_CLIENTS=50
WORD="palavra_teste"  # Defina a palavra que você quer enviar

# Medir o tempo de execução das conexões de strings
echo "Medindo tempo para conexões de strings..."
time {
  for ((i = 1; i <= NUM_CLIENTS; i++))
  do
    echo "$WORD" | ./string_consumer & 
  done
  wait  # Espera que todos os clientes finalizem

# Medir o tempo de execução das conexões de números
echo "Medindo tempo para conexões de números..."
  for ((i = 1; i <= NUM_CLIENTS; i++))
  do
    echo $i | ./number_consumer & 
  done
  wait  # Espera que todos os clientes finalizem
}