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
    printf("Processando dados: %s", task->buffer);

    sleep(5);

    // Processar os dados (por exemplo, converter para maiúsculas)
    for (int i = 0; i < strlen(task->buffer); i++) {
        task->buffer[i] = toupper(task->buffer[i]);
    }

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

int main(int argc, char *argv[]){

    pthread_t threads[THREAD_NUM];

    int str_sockfd;
    struct sockaddr_un local_str;

    pthread_mutex_init(&mutexQueue, NULL);
    pthread_cond_init(&condQueue, NULL);

    // Criar socket para strings
    str_sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (str_sockfd < 0) {
        perror("Falha ao criar socket para strings");
        return 1;
    }

    // Bind no socket (para strings)
    memset(&local_str, 0, sizeof(local_str));
    local_str.sun_family = AF_UNIX;
    strncpy(local_str.sun_path, STR_SOCK_PATH, sizeof(local_str.sun_path) - 1);
    unlink(local_str.sun_path);
    int len_strings = strlen(local_str.sun_path) + sizeof(local_str.sun_family);
    if (bind(str_sockfd, (struct sockaddr*)&local_str, len_strings) < 0) {
        perror("Falha ao fazer bind no socket de strings");
        close(str_sockfd);
        return 1;
    }

    // Listen for connections
    if (listen(str_sockfd, 5) < 0) {
        perror("Erro ao escutar socket de strings");
        close(str_sockfd);
        return 1;
    }

    printf("Servidor ouvindo em %s...\n", /*NUM_SOCK_PATH,*/ STR_SOCK_PATH);

    // Criando pool de threads
    for(int i = 0; i < THREAD_NUM; i++){
        if(pthread_create(&threads[i], NULL, &startThread, NULL) != 0){
            printf("Erro ao criar thread\n");
        }
    }

    // Aceitando conexões
    while(1){
        handleConnection(str_sockfd);
    }

    // Limpar recursos (nunca vai chegar aqui no loop infinito)
    close(str_sockfd);
    pthread_mutex_destroy(&mutexQueue);
    pthread_cond_destroy(&condQueue);
    return 0;
}