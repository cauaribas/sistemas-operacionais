#include <windows.h>
#include <stdio.h>

#define PIPE_NAME "\\\\.\\pipe\\string_pipe"

int main()
{
    HANDLE pipe;
    char buffer[1024];
    DWORD bytesWritten, bytesRead;

    pipe = CreateFile(
        PIPE_NAME,
        GENERIC_READ | GENERIC_WRITE,
        0, NULL, OPEN_EXISTING, 0, NULL);

    if (pipe == INVALID_HANDLE_VALUE)
    {
        printf("Erro ao conectar ao servidor.\n");
        return 1;
    }

    printf("Digite o dado a ser enviado: ");
    fgets(buffer, sizeof(buffer), stdin);

    WriteFile(pipe, buffer, strlen(buffer) + 1, &bytesWritten, NULL);
    ReadFile(pipe, buffer, sizeof(buffer), &bytesRead, NULL);

    printf("Dado recebido: %s", buffer);

    CloseHandle(pipe);

    return 0;
}
