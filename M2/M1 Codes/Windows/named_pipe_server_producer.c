#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define THREAD_POOL_SIZE 1024
#define MAX_PIPE_INSTANCES PIPE_UNLIMITED_INSTANCES
#define STRING_PIPE_NAME "\\\\.\\pipe\\string_pipe"
#define NUMBER_PIPE_NAME "\\\\.\\pipe\\number_pipe"

typedef struct Task
{
    void (*taskFunction)(HANDLE);
    HANDLE client_pipe;
} Task;

typedef struct AcceptParams
{
    const char *pipeName;
    void (*handler)(HANDLE);
} AcceptParams;

Task taskQueue[256];
int taskCount = 0;
HANDLE mutexQueue;
HANDLE condQueue;

void executeTask(Task *task)
{
    task->taskFunction(task->client_pipe);
}

void submitTask(Task task)
{
    WaitForSingleObject(mutexQueue, INFINITE);
    taskQueue[taskCount++] = task;
    ReleaseMutex(mutexQueue);
    ReleaseSemaphore(condQueue, 1, NULL);
}

DWORD WINAPI startThread(LPVOID args)
{
    while (1)
    {
        Task task;
        WaitForSingleObject(condQueue, INFINITE);
        WaitForSingleObject(mutexQueue, INFINITE);
        task = taskQueue[--taskCount];
        ReleaseMutex(mutexQueue);
        executeTask(&task);
    }
    return 0;
}

void handleStringConnection(HANDLE client_pipe)
{
    char buffer[1024];
    DWORD bytesRead;

    if (ReadFile(client_pipe, buffer, sizeof(buffer), &bytesRead, NULL))
    {
        buffer[bytesRead] = '\0';

        for (DWORD i = 0; i < bytesRead; i++)
        {
            buffer[i] = toupper(buffer[i]);
        }

        DWORD bytesWritten;
        WriteFile(client_pipe, buffer, strlen(buffer) + 1, &bytesWritten, NULL);
        printf("Conexão de string processada.\n");
    }
    else
    {
        printf("Erro ao ler do cliente (string). Código do erro: %lu\n", GetLastError());
    }
    CloseHandle(client_pipe);
}

void handleNumberConnection(HANDLE client_pipe)
{
    char buffer[1024];
    DWORD bytesRead;

    if (ReadFile(client_pipe, buffer, sizeof(buffer), &bytesRead, NULL))
    {
        buffer[bytesRead] = '\0';

        int number = atoi(buffer);
        number *= 2;

        snprintf(buffer, sizeof(buffer), "%d", number);

        DWORD bytesWritten;
        WriteFile(client_pipe, buffer, strlen(buffer) + 1, &bytesWritten, NULL);
        printf("Conexão de número processada.\n");
    }
    else
    {
        printf("Erro ao ler do cliente (número). Código do erro: %lu\n", GetLastError());
    }
    CloseHandle(client_pipe);
}

HANDLE createPipeServer(const char *pipeName)
{
    HANDLE pipe = CreateNamedPipe(
        pipeName,
        PIPE_ACCESS_DUPLEX,
        PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
        MAX_PIPE_INSTANCES,
        1024, 1024, 0, NULL);

    if (pipe == INVALID_HANDLE_VALUE)
    {
        printf("Erro ao criar o pipe %s. Código do erro: %lu\n", pipeName, GetLastError());
    }
    return pipe;
}

DWORD WINAPI acceptConnections(LPVOID param)
{
    AcceptParams *params = (AcceptParams *)param;
    const char *pipeName = params->pipeName;
    void (*handler)(HANDLE) = params->handler;
    HANDLE pipe;

    while (1)
    {
        pipe = createPipeServer(pipeName);
        if (pipe == INVALID_HANDLE_VALUE)
        {
            printf("Erro ao criar o pipe %s. Código do erro: %lu\n", pipeName, GetLastError());
            Sleep(1000);
            continue;
        }

        printf("Aguardando conexão no pipe %s...\n", pipeName);
        if (ConnectNamedPipe(pipe, NULL) || GetLastError() == ERROR_PIPE_CONNECTED)
        {
            printf("Nova conexão estabelecida no pipe %s\n", pipeName);
            Task task = {.taskFunction = handler, .client_pipe = pipe};
            submitTask(task);
        }
        else
        {
            printf("Falha ao conectar ao pipe %s. Código do erro: %lu\n", pipeName, GetLastError());
            CloseHandle(pipe);
        }
    }

    return 0;
}

int main()
{
    DWORD pid = GetCurrentProcessId();
    printf("Servidor iniciado. PID: %lu\n", pid);

    mutexQueue = CreateMutex(NULL, FALSE, NULL);
    condQueue = CreateSemaphore(NULL, 0, THREAD_POOL_SIZE, NULL);

    HANDLE th_pool[THREAD_POOL_SIZE];
    for (int i = 0; i < THREAD_POOL_SIZE; i++)
    {
        th_pool[i] = CreateThread(NULL, 0, startThread, NULL, 0, NULL);
    }

    printf("Servidor iniciando...\n");

    AcceptParams stringParams = {STRING_PIPE_NAME, handleStringConnection};
    AcceptParams numberParams = {NUMBER_PIPE_NAME, handleNumberConnection};

    HANDLE stringThread = CreateThread(NULL, 0, acceptConnections, &stringParams, 0, NULL);
    HANDLE numberThread = CreateThread(NULL, 0, acceptConnections, &numberParams, 0, NULL);

    WaitForSingleObject(stringThread, INFINITE);
    WaitForSingleObject(numberThread, INFINITE);

    return 0;
}
