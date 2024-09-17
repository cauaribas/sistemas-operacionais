#!/bin/bash

NUM_CLIENTS=1000
WORD="palavra_teste"  # Defina a palavra que você quer enviar
ITERATIONS=10
TOTAL_TIME=0

for ((n = 1; n <= ITERATIONS; n++))
do
  # Executa o comando e captura o tempo real (real) de execução
  echo "Execução $n"
  EXEC_TIME=$( { time ( 
    for ((i = 1; i <= NUM_CLIENTS; i++))
    do
      echo "$WORD" | ./string_consumer &
      echo $i | ./number_consumer &
    done
    wait
  ) 2>&1; } | grep real | awk '{print $2}' )

  # Converte o tempo de minutos:segundos.milisegundos para segundos
  MIN=$(echo $EXEC_TIME | awk -F'm' '{print $1}')
  SEC=$(echo $EXEC_TIME | awk -F'm' '{print $2}' | sed 's/s//')
  EXEC_TIME_SEC=$(echo "$MIN * 60 + $SEC" | bc)
  
  # Soma ao tempo total
  TOTAL_TIME=$(echo "$TOTAL_TIME + $EXEC_TIME_SEC" | bc)
  echo "Tempo da execução $n: $EXEC_TIME_SEC segundos"
done

# Calcula a média
AVERAGE_TIME=$(echo "$TOTAL_TIME / $ITERATIONS" | bc -l)
echo "Média de tempo após $ITERATIONS execuções: $AVERAGE_TIME segundos"