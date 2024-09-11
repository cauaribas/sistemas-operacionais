#!/bin/bash

NUM_CLIENTS=50 

for ((i = 1; i <= NUM_CLIENTS; i++))
do
  echo "Enviando dados do cliente $i" | ./str_consumer & 
done

for ((i = 1; i <= NUM_CLIENTS; i++))
do
  echo "Enviando dados do cliente $i" | ./num_consumer & 
done

wait  # Espera que todos os clientes finalizem antes de sair