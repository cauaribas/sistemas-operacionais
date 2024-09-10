#!/bin/bash

NUM_CLIENTS=50  # Número de clientes a serem executados simultaneamente

for ((i = 1; i <= NUM_CLIENTS; i++))
do
  echo "Enviando dados do cliente $i" | ./s_consumer &  # Substitua ./client pelo caminho do seu cliente
done

wait  # Espera que todos os clientes finalizem antes de sair