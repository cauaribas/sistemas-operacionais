/**
 * 1. Implemente um sistema em que há três threads que tem a responsabilidade de ler o valor de um sensor e some ao valor de uma variável global e em uma variável local. 
 * Você deve simular a contagem com operação de incremento simples (+=1 ou ++). Print a variável local a cada 1 segundo e incremente a variável a cada 1 segundo. 
 * O programa deve chegar ao fim quando a soma da variável global chegar a 100. Ao fim, você consegue observar qual condição:
 * 1. Todas as threads tem o mesmo valor na variável interna?
 * Não, pois cada thread tem sua variável local e nem todas as threads terão o mesmo valor da variável interna (local) ao final da execução
 * 2. O print da variável global segue um incremento linear?
 * Não, o print da variável global provavelmente não segue um incremento linear por causa da concorrência entre as threads.
 */

#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

pthread_mutex_t exclusao_mutua = PTHREAD_MUTEX_INITIALIZER;

// pthread_mutex_lock(&exclusao_mutua);
// pthread_mutex_unlock(&exclusao_mutua);

int global = 0;

void *sensor_read(void *arg){
    int thread_id = *(int*)arg;
    int local = 0;
    while(1){
        local += 1;
        pthread_mutex_lock(&exclusao_mutua);
        if(global < 100){
            global++;
        } else {
            pthread_exit(0);
        }
        
        pthread_mutex_unlock(&exclusao_mutua);

        sleep(1);
        
        printf("Thread %d - Local: %d\n", thread_id, local);
        printf("Thread %d - Global: %d\n", thread_id, global);
    }
    pthread_exit(0);
}

int main(int argc, char *argv[]){
    pthread_t t1, t2, t3;
    int id1 = 1, id2 = 2, id3 = 3;

    pthread_create(&t1,NULL,sensor_read,&id1);
    pthread_create(&t2,NULL,sensor_read,&id2);
    pthread_create(&t3,NULL,sensor_read,&id3);

    pthread_join(t1,NULL);
    pthread_join(t2,NULL);
    pthread_join(t3,NULL);
}