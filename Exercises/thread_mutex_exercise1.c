/**
 * 1. Desenvolver uma aplicação de leia uma entrada do teclado, some com uma constante e imprima na tela o dado resultante da soma. 
 * Divida as tarefas em threads usando a biblioteca pthread (preferencialmente).
 *
 * gcc -o <.exe_name> <file_name> -lpthread
 * ./<.exe_name>
 *
 * pthread_t tid1, tid2, tid3;
 * pthread_attr_t attr;
 * pthread_create(&tid,&attr,<function>,argv[1]);
 * pthread_join(tid,NULL);
 */

#include <pthread.h>
#include <stdio.h>

pthread_mutex_t exclusao_mutua = PTHREAD_MUTEX_INITIALIZER; //LOCK da seção crítica

int sum = 0;
int const num = 10;

void *thread_input(void *param){
    pthread_mutex_lock(&exclusao_mutua);
    
    printf("\nValue to be summed: ");
    scanf("%d", &sum);
    
    pthread_mutex_unlock( &exclusao_mutua);
    pthread_exit(0);
}

void *thread_calculator(void *param){
    pthread_mutex_lock(&exclusao_mutua);
    
    sum = sum + num;

    pthread_mutex_unlock( &exclusao_mutua);
    pthread_exit(0);
}

void *thread_print(void *param){
    pthread_mutex_lock(&exclusao_mutua);
    
    printf("\nResult: %d\n", sum);

    pthread_mutex_unlock( &exclusao_mutua);
    pthread_exit(0);
}

int main(int argc, char *argv[]){

    pthread_t tid_input, tid_sum, tid_print;
    pthread_attr_t attr;

    /* get the default attributes */
    pthread_attr_init(&attr);

    /* create the threads */
    pthread_create(&tid_input,&attr,thread_input,NULL);
    pthread_create(&tid_sum,&attr,thread_calculator,NULL);
    pthread_create(&tid_print,&attr,thread_print,NULL);

    /* now wait for the threads to exit */
    pthread_join(tid_input,NULL);
    pthread_join(tid_sum,NULL);
    pthread_join(tid_print,NULL);
}