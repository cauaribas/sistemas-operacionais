#!/bin/bash

NUM_CLIENTS=10
WORD="palavra_teste"  # Defina a palavra que você quer enviar

for ((i = 1; i <= NUM_CLIENTS; i++))
do
  echo "Enviando palavra do cliente $i: $WORD" | ./str_consumer & 
done

for ((i = 1; i <= NUM_CLIENTS; i++))
do
  echo "Enviando dados do cliente $i: $i" | ./num_consumer & 
done

wait  # Espera que todos os clientes finalizem antes de sair