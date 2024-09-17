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

// Constantes
#define THREAD_POOL_SIZE 4
#define STRING_SOCK_PATH "/tmp/string_pipeso"
#define NUMBER_SOCK_PATH "/tmp/number_pipeso"

// Struct para armazenar as tarefas a serem executadas por threads, contendo a função e o socket do cliente
typedef struct Task {
    void (*taskFunction)(int);
    int client_socket;
} Task;

// Variáveis globais compartilhadas entre threads
Task taskQueue[256];
int taskCount = 0;
pthread_mutex_t mutexQueue;
pthread_cond_t condQueue;

void executeTask(Task* task){
    task->taskFunction(task->client_socket); // Executa a função de tarefa associada
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
        while(taskCount == 0){ // Se não há tarefas, espera até ser sinalizado
            pthread_cond_wait(&condQueue, &mutexQueue);
        }

        // Retira a primeira tarefa da fila
        task = taskQueue[0];
        for(int i = 0; i < taskCount - 1; i++){
            taskQueue[i] = taskQueue[i + 1];
        }
        taskCount--;
        pthread_mutex_unlock(&mutexQueue);
        
        executeTask(&task); // Executa a tarefa removida da fila
    }
}

int startServer(const char* sockpath){
    int server_socket;
    struct sockaddr_un local;

    // Criação do socket UNIX
    server_socket = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_socket < 0) {
        perror("Falha ao criar socket");
        return 1;
    }

    // Bind socket no endereço local
    memset(&local, 0, sizeof(local));
    local.sun_family = AF_UNIX;
    strncpy(local.sun_path, sockpath, sizeof(local.sun_path) - 1); // Define o caminho do socket
    unlink(local.sun_path); // Remove qualquer socket anterior com o mesmo caminho
    int len = strlen(local.sun_path) + sizeof(local.sun_family);
    
    // Vincula o socket ao caminho especificado
    if (bind(server_socket, (struct sockaddr*)&local, len) < 0) {
        perror("Falha ao fazer bind no socket");
        close(server_socket);
        return 1;
    }

    // Coloca o socket em modo de escuta por conexões
    if (listen(server_socket, 5) < 0) {
        perror("Erro ao escutar socket");
        close(server_socket);
        return 1;
    }

    return server_socket;
}

void handleStringConnection(int client_socket){    
    char buffer[1024];

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
        printf("Nova conexão estabelecida\n");
        
        // Cria uma nova tarefa e a submete para o pool de threads
        Task t = {
            .taskFunction = handler, 
            .client_socket = client_socket
        };

        submitTask(t);
    }
}

int main(int argc, char* argv[]){
    pthread_t th_pool[THREAD_POOL_SIZE]; // Array de threads
    int string_socket, number_socket;
    
    pthread_mutex_init(&mutexQueue, NULL);
    pthread_cond_init(&condQueue, NULL);

    // Cria as threads do pool
    for(int i = 0; i < THREAD_POOL_SIZE; i++){
        if(pthread_create(&th_pool[i], NULL, &startThread, NULL)){
            perror("Failed to create thread");
        }
    }
    
    // Inicia os servidores para strings e números
    string_socket = startServer(STRING_SOCK_PATH);
    number_socket = startServer(NUMBER_SOCK_PATH);

    printf("Servidor escutando em %s e %s...\n", STRING_SOCK_PATH, NUMBER_SOCK_PATH);

    // Define variáveis para o select e determina o maior descritor de arquivo
    fd_set read_fds;
    int max_fd = (string_socket > number_socket) ? string_socket : number_socket;

    while(1){
        FD_ZERO(&read_fds); // Limpa o conjunto de descritores
        FD_SET(string_socket, &read_fds); // Adiciona string_socket ao conjunto
        FD_SET(number_socket, &read_fds); // Adiciona number_socket ao conjunto

        // Espera por atividade nos sockets com select
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
    
    // Limpando recursos (devido ao loop infinito, nunca chegará aqui)
    pthread_mutex_destroy(&mutexQueue);
    pthread_cond_destroy(&condQueue);
    close(string_socket);
    close(number_socket);
    return 0;
}