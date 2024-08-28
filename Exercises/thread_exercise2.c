/**
 * 2. Faça uma aplicação que tenha um vetor de 10 valores, gerados randomicamente ou com entrada do usuário. 
 * Com o vetor preenchido, eles devem gerar uma soma e um produto (resultado de uma multiplicação). 
 * Você deve usar pelo menos duas threads para cada operação (soma e multiplicação) e utilizar os dados no vetor original.
 * pthread_t tid1, tid2, tid3;
 * pthread_attr_t attr;
 * pthread_create(&tid,&attr,<function>,argv[1]);
 * pthread_join(tid,NULL);
 */

#include <pthread.h>
#include <stdio.h>

int array[10] = {1,2,3,4,5,6,7,8,9,10};
int sum1 = 0;
int sum2 = 0;
int mul1 = 1;
int mul2 = 1;

void *thread_sum1(void *param){
    for(int i = 0; i < 5; i++){
        sum1 += array[i];
    }
    pthread_exit(0);
}

void *thread_sum2(void *param){
    for(int i = 5; i < 10; i++){
        sum2 += array[i];
    }
    pthread_exit(0);
}

void *thread_mul1(void *param){
    for(int i = 0; i < 5; i++){
        mul1 *= array[i];
    }
    pthread_exit(0);
}

void *thread_mul2(void *param){
    for(int i = 5; i < 10; i++){
        mul2 *= array[i];
    }
    pthread_exit(0);
}

int main(int argc, char *argv[]){

    pthread_t tid_sum1, tid_sum2, tid_mul1, tid_mul2;
    pthread_attr_t attr;

    pthread_attr_init(&attr);

    /* create the threads */
    pthread_create(&tid_sum1,&attr,thread_sum1,NULL);
    pthread_create(&tid_sum2,&attr,thread_sum2,NULL);

    pthread_create(&tid_mul1,&attr,thread_mul1,NULL);
    pthread_create(&tid_mul2,&attr,thread_mul2,NULL);

    /* now wait for the threads to exit */
    pthread_join(tid_sum1,NULL);
    pthread_join(tid_sum2,NULL);

    pthread_join(tid_mul1,NULL);
    pthread_join(tid_mul2,NULL);
    
    printf("Final sum: %d\n", sum1 + sum2);
    printf("Final product: %d\n", mul1 * mul2);
}