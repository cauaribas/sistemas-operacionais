#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#define THREAD_NUM 4 
#define MAX_CLIENTS 10

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
pthread_mutex_t clientQueue;
pthread_cond_t condQueue;

void executeTask(Task* task){

    sleep(2);

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

        // executeTask(&task);
    }
}

void handleNumberConnection(int sockfd){
    struct sockaddr_un remote;
    int len;
    int client_socket;

    sleep(2);
    Task newTask;
    int number;

    // Aceitar conexão
    memset(&remote, 0, sizeof(remote));
    len = sizeof(remote);
    client_socket = accept(sockfd, (struct sockaddr*)&remote, &len);
    if (client_socket < 0) {
        perror("Falha ao aceitar conexão");
        return;
    }

    printf("Cliente conectado!(Number)\n");

    // Ler dados do cliente
    memset(newTask.buffer, 0, sizeof(newTask.buffer));
    newTask.sockfd = client_socket;
    if (read(client_socket, newTask.buffer, sizeof(newTask.buffer)) < 0) {
        perror("Erro ao ler do socket");
        close(client_socket);
        return;
    }

    // Enviar a tarefa para a pool de threads
    submitTask(newTask);

    number = atoi(newTask.buffer);
    number *= 2;
    snprintf(newTask.buffer, sizeof(newTask.buffer), "%d", number);

    // Enviar dados de volta ao cliente
    write(newTask.sockfd, newTask.buffer, strlen(newTask.buffer) + 1);
    close(newTask.sockfd);
}

void handleStringConnection(int sockfd){
    struct sockaddr_un remote;
    int len;
    int client_socket;

    Task newTask;
    
    // Aceitar conexão
    memset(&remote, 0, sizeof(remote));
    len = sizeof(remote);
    client_socket = accept(sockfd, (struct sockaddr*)&remote, &len);
    if (client_socket < 0) {
        perror("Falha ao aceitar conexão");
        return;
    }

    printf("Cliente conectado!(String)\n");

    // Ler dados do cliente
    memset(newTask.buffer, 0, sizeof(newTask.buffer));
    newTask.sockfd = client_socket;
    if (read(client_socket, newTask.buffer, sizeof(newTask.buffer)) < 0) {
        perror("Erro ao ler do socket");
        close(client_socket);
        return;
    }

    // Enviar a tarefa para a pool de threads
    submitTask(newTask);

    for (int i = 0; i < strlen(newTask.buffer); i++)
    {
        newTask.buffer[i] = toupper(newTask.buffer[i]);
    }

    // Enviar dados de volta ao cliente
    write(newTask.sockfd, newTask.buffer, strlen(newTask.buffer) + 1);
    close(newTask.sockfd);
}

int serverSocket(const char* sockpath){
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

int main(int argc, char *argv[]){
    int str_socket, num_socket;
    pthread_t threads[THREAD_NUM];

    pthread_mutex_init(&mutexQueue, NULL);
    pthread_mutex_init(&clientQueue, NULL);
    pthread_cond_init(&condQueue, NULL);

    // Criando pool de threads
    for(int i = 0; i < THREAD_NUM; i++){
        if(pthread_create(&threads[i], NULL, &startThread, NULL) != 0){
            printf("Erro ao criar thread\n");
        }
    }

    // Criar sockets
    str_socket = serverSocket(STR_SOCK_PATH);
    num_socket = serverSocket(NUM_SOCK_PATH);

    printf("Servidor ouvindo em %s e %s...\n", STR_SOCK_PATH, NUM_SOCK_PATH);

    // Aceitando conexões
    while(1){
        handleStringConnection(str_socket);
        handleNumberConnection(num_socket);
    }

    // Limpar recursos (nunca vai chegar aqui no loop infinito)
    close(str_socket);
    close(num_socket);

    pthread_mutex_destroy(&mutexQueue);
    pthread_cond_destroy(&condQueue);
    return 0;
}