#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#define THREAD_POOL_SIZE 4
#define MAX_CLIENTS 10

#define STRING_SOCK_PATH "/tmp/string_pipeso"
#define NUMBER_SOCK_PATH "/tmp/number_pipeso"

typedef struct Task {
    void (*taskFunction)();
    // int arg1, arg2;
} Task;

Task taskQueue[256];
int taskCount = 0;

pthread_mutex_t mutexQueue;
pthread_cond_t condQueue;

void product(int a, int b){
    sleep(2);
    int sum = a + b;
    int product = a * b;
    printf("Product of %d and %d is%d\n",a, b, product);
}

void sum(int a, int b){
    sleep(2);
    int sum = a + b;
    printf("Sum of %d and %d is %d\n", a, b, sum);
}

void executeTask(Task* task){
    // sleep(1);
    // int result = task->a + task->b;
    // printf("Task: %d + %d = %d\n", task->a, task->b, result);
    task->taskFunction(task->arg1, task->arg2);
}

void submitTask(Task task){
    pthread_mutex_lock(&mutexQueue);
    taskQueue[taskCount] = task;
    taskCount++;
    pthread_mutex_unlock(&mutexQueue);
    pthread_cond_signal(&condQueue);
}

void* startThread(void* args){
    while(1){
        Task task;

        pthread_mutex_lock(&mutexQueue);
        while(taskCount == 0){
            pthread_cond_wait(&condQueue, &mutexQueue);
        }

        task = taskQueue[0];
        for(int i = 0; i < taskCount - 1; i++){
            taskQueue[i] = taskQueue[i + 1];
        }
        taskCount--;
        pthread_mutex_unlock(&mutexQueue);
        
        executeTask(&task); 
    }
}

int startServer(const char* path){
    int server_socket;
    struct sockaddr_un local;

    server_socket = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_socket < 0) {
        perror("Falha ao criar socket");
        return 1;
    }

    memset(&local, 0, sizeof(local));
    local.sun_family = AF_UNIX;
    strncpy(local.sun_path, sockpath, sizeof(local.sun_path) - 1);
    unlink(local.sun_path);
    int len = strlen(local.sun_path) + sizeof(local.sun_family);
    
    if (bind(server_socket, (struct sockaddr*)&local, len) < 0) {
        perror("Falha ao fazer bind no socket");
        close(server_socket);
        return 1;
    }

    if (listen(server_socket, 5) < 0) {
        perror("Erro ao escutar socket");
        close(server_socket);
        return 1;
    }

    return server_socket;
}

int main(int argc, char* argv[]){
    
    pthread_t th_pool[THREAD_POOL_SIZE];
    int string_socket, number_socket, client_socket;

    pthread_mutex_init(&mutexQueue, NULL);
    pthread_cond_init(&condQueue, NULL);

    for(int i = 0; i < THREAD_POOL_SIZE; i++){
        if(pthread_create(&th_pool[i], NULL, &startThread, NULL)){
            perror("Failed to create thread");
        }
    }

    string_socket = startServer(string_socket);
    number_socket = startServer(number_socket);

    // to do: finish seting up both server sockets

    Task t = {
        .taskFunction = &handleNumberConnection
    };
    submitTask(t);
    
    for(int i = 0; i < THREAD_POOL_SIZE; i++){
        if(pthread_join(th_pool[i], NULL)){
            perror("Failed to join thread");
        }
    }
    pthread_mutex_destroy(&mutexQueue);
    pthread_cond_destroy(&condQueue);
    return 0;
}