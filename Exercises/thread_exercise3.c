#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <pthread.h> 

#define COUNT_READ 10
int var_global = 0;

void *th_sensor1(void *param){
    int var_local = 0;
    while(var_global<COUNT_READ){
        var_local++;
        var_global++;
        usleep(1000*1000);
        printf("Var local th1: %d\n", var_local);
    }
    pthread_exit(0);
}

void *th_sensor2(void *param){
    int var_local = 0;
    while(var_global<COUNT_READ){
        var_local++;
        var_global++;
        usleep(1000*1000);
        printf("Var local th2: %d\n", var_local);
    }
    pthread_exit(0);
}

void *th_sensor3(void *param){
    int var_local = 0;
    while(var_global<COUNT_READ){
        var_global++;
        var_local++;
        usleep(1000*1000);
        printf("Var local th3: %d\n", var_local);
    }
    pthread_exit(0);
}


int main(){ 
    pthread_t th_s1, th_s2, th_s3;
    pthread_attr_t attr;

    pthread_attr_init(&attr);

    pthread_create(&th_s1, &attr, th_sensor1, NULL);
    pthread_create(&th_s2, &attr, th_sensor2, NULL);
    pthread_create(&th_s3, &attr, th_sensor3, NULL);

    pthread_join(th_s1, NULL);
    pthread_join(th_s2, NULL);
    pthread_join(th_s3, NULL);

    printf("Var global: %d\n", var_global);
}