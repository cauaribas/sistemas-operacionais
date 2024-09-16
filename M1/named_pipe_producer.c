#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/select.h>

#define THREAD_POOL_SIZE 2
#define STRING_SOCK_PATH "/tmp/string_pipeso"
#define NUMBER_SOCK_PATH "/tmp/number_pipeso"

typedef struct Task {
    void (*taskFunction)(int);
    int client_socket;
} Task;

Task taskQueue[256];
int taskCount = 0;

pthread_mutex_t mutexQueue;
pthread_cond_t condQueue;

struct timespec startTime, endTime;

void executeTask(Task* task){
    task->taskFunction(task->client_socket);
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
        // printf("Task removida. Tarefas restantes na fila: %d\n", taskCount);
        pthread_mutex_unlock(&mutexQueue);
        
        executeTask(&task); 
    }
}

int startServer(const char* sockpath){
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

void handleStringConnection(int client_socket){    
    char buffer[1024];
    // sleep(2);

    // Ler dados do cliente
    if (read(client_socket, buffer, sizeof(buffer)) < 0) {
        perror("Erro ao ler do socket");
        close(client_socket);
        return;
    }

    // Processa a string recebida e converte para maiúsculas
    for (int i = 0; i < strlen(buffer); i++){
        buffer[i] = toupper(buffer[i]);
    }

    // Enviar dados de volta ao cliente
    write(client_socket, buffer, strlen(buffer) + 1);
    close(client_socket);
}

void handleNumberConnection(int client_socket){    
    char buffer[1024];
    // sleep(2);

    // Ler dados do cliente
    if (read(client_socket, buffer, sizeof(buffer)) < 0) {
        perror("Erro ao ler do socket");
        close(client_socket);
        return;
    }

    // Processa o numero recebido e multiplica por 2
    int number = atoi(buffer);
    number *= 2;

    snprintf(buffer, sizeof(buffer), "%d", number);

    // Enviar dados de volta ao cliente
    write(client_socket, buffer, strlen(buffer) + 1);
    close(client_socket);
}

void acceptConnection(int socket, void (*handler)(int)){
    struct sockaddr_un remote;
    socklen_t len = sizeof(remote);
    int client_socket = accept(socket, (struct sockaddr*)&remote, &len);
    if (client_socket >= 0) {
        printf("Nova conexão recebida\n");
        
        Task t = {
            .taskFunction = handler, 
            .client_socket = client_socket
        };

        submitTask(t);
    }
}

int main(int argc, char* argv[]){
    pthread_t th_pool[THREAD_POOL_SIZE];
    int string_socket, number_socket;
    
    pthread_mutex_init(&mutexQueue, NULL);
    pthread_cond_init(&condQueue, NULL);

    for(int i = 0; i < THREAD_POOL_SIZE; i++){
        if(pthread_create(&th_pool[i], NULL, &startThread, NULL)){
            perror("Failed to create thread");
        }
    }
    
    string_socket = startServer(STRING_SOCK_PATH);
    number_socket = startServer(NUMBER_SOCK_PATH);

    printf("Servidor escutando em %s e %s...\n", STRING_SOCK_PATH, NUMBER_SOCK_PATH);

    clock_gettime(CLOCK_MONOTONIC, &startTime);

    fd_set read_fds;
    int max_fd = (string_socket > number_socket) ? string_socket : number_socket;

    while(1){
        FD_ZERO(&read_fds); // Limpa o conjunto de descritores
        FD_SET(string_socket, &read_fds); // Adiciona string_socket ao conjunto
        FD_SET(number_socket, &read_fds); // Adiciona number_socket ao conjunto

        int activity = select(max_fd + 1, &read_fds, NULL, NULL, NULL);
        if (activity < 0) {
            perror("Erro no select");
            continue;
        }
        // Verifica se há conexão no socket de strings
        if(FD_ISSET(string_socket, &read_fds)){
            acceptConnection(string_socket, handleStringConnection);
        }
        // Verifica se há conexão no socket de números
        if(FD_ISSET(number_socket, &read_fds)){
            acceptConnection(number_socket, handleNumberConnection);
        }
    }
    
    pthread_mutex_destroy(&mutexQueue);
    pthread_cond_destroy(&condQueue);

    close(string_socket);
    close(number_socket);
    return 0;
}