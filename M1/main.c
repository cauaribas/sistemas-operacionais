#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <ctype.h>

#define THREAD_NUM 4 // Numero de threads na pool 
#define NUM_SOCK_PATH "/tmp/num_pipeso"
#define STR_SOCK_PATH "/tmp/str_pipeso"

typedef struct Task {
    int sockfd;
    char buffer[1024];
} Task;

Task taskQueue[256];
int taskCount = 0;
int clientCount = 0;

pthread_mutex_t mutexQueue;
pthread_cond_t condQueue;

void executeTask(Task* task){

    sleep(5);

    // Enviar dados de volta ao cliente
    write(task->sockfd, task->buffer, strlen(task->buffer) + 1);

    close(task->sockfd); // Fecha a conexão
}

void submitTask(Task task){
    pthread_mutex_lock(&mutexQueue);
    taskQueue[taskCount] = task;
    taskCount++;
    pthread_mutex_unlock(&mutexQueue);
    pthread_cond_signal(&condQueue); // Sinalizar para a thread em espera que uma tarefa está disponível
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

void handleConnection(int sockfd){
    struct sockaddr_un remote;
    int len;
    
    int newsockfd;
    Task newTask;
    
    // Aceitar conexão
    memset(&remote, 0, sizeof(remote));
    len = sizeof(remote);
    newsockfd = accept(sockfd, (struct sockaddr*)&remote, &len);
    if (newsockfd < 0) {
        perror("Falha ao aceitar conexão");
        return;
    }

    printf("Cliente conectado!\n");

    // Ler dados do cliente
    memset(newTask.buffer, 0, sizeof(newTask.buffer));
    newTask.sockfd = newsockfd;
    if (read(newsockfd, newTask.buffer, sizeof(newTask.buffer)) < 0) {
        perror("Erro ao ler do socket");
        close(newsockfd);
        return;
    }

    // Enviar a tarefa para a pool de threads
    submitTask(newTask);
}

int serverSocket(const char* sockpath){
    int server_sockfd;
    struct sockaddr_un local;

    server_sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_sockfd < 0) {
        perror("Falha ao criar socket");
        return 1;
    }

    memset(&local, 0, sizeof(local));
    local.sun_family = AF_UNIX;
    strncpy(local.sun_path, sockpath, sizeof(local.sun_path) - 1);
    unlink(local.sun_path);
    int len = strlen(local.sun_path) + sizeof(local.sun_family);
    
    if (bind(server_sockfd, (struct sockaddr*)&local, len) < 0) {
        perror("Falha ao fazer bind no socket");
        close(server_sockfd);
        return 1;
    }

    if (listen(server_sockfd, 5) < 0) {
        perror("Erro ao escutar socket");
        close(server_sockfd);
        return 1;
    }

    return server_sockfd;
}

int main(int argc, char *argv[]){

    struct sockaddr_un remote;
    int str_sockfd, num_sockfd;
    pthread_t threads[THREAD_NUM];

    pthread_mutex_init(&mutexQueue, NULL);
    pthread_cond_init(&condQueue, NULL);

    // Criar sockets
    str_sockfd = serverSocket(STR_SOCK_PATH);
    num_sockfd = serverSocket(NUM_SOCK_PATH);

    printf("Servidor ouvindo em %s e %s...\n", STR_SOCK_PATH, NUM_SOCK_PATH);

    // Criando pool de threads
    for(int i = 0; i < THREAD_NUM; i++){
        if(pthread_create(&threads[i], NULL, &startThread, NULL) != 0){
            printf("Erro ao criar thread\n");
        }
    }

    // Aceitando conexões
    while(1){
        handleConnection(str_sockfd);
        // handleConnection(num_sockfd); // fix: preciso arrumar para aceitar conexoes de ambos os sockets
    }

    // Limpar recursos (nunca vai chegar aqui no loop infinito)
    close(str_sockfd);
    close(num_sockfd);

    pthread_mutex_destroy(&mutexQueue);
    pthread_cond_destroy(&condQueue);
    return 0;
}