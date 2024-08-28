/**
 * 1. Implemente um sistema em que há três threads que tem a responsabilidade de ler o valor de um sensor e some ao valor de uma variável global e em uma variável local. 
 * Você deve simular a contagem com operação de incremento simples (+=1 ou ++). Print a variável local a cada 1 segundo e incremente a variável a cada 1 segundo. 
 * O programa deve chegar ao fim quando a soma da variável global chegar a 100. Ao fim, você consegue observar qual condição:
 * 1. Todas as threads tem o mesmo valor na variável interna?
 * 2. O print da variável global segue um incremento linear?
 */

#include <pthread.h>
#include <stdio.h>