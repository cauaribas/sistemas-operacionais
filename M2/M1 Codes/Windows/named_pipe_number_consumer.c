#include <windows.h>
#include <stdio.h>
#include <string.h>

#define PIPE_NAME "\\\\.\\pipe\\number_pipe"

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
        printf("Erro ao conectar ao servidor. Código do erro: %lu\n", GetLastError());
        return 1;
    }

    printf("Digite o dado a ser enviado: ");
    fgets(buffer, sizeof(buffer), stdin);

    // Remove o caractere de nova linha, se presente
    buffer[strcspn(buffer, "\n")] = 0;

    // Verifica se a escrita no pipe foi bem-sucedida
    if (!WriteFile(pipe, buffer, strlen(buffer) + 1, &bytesWritten, NULL))
    {
        printf("Erro ao escrever no pipe. Código do erro: %lu\n", GetLastError());
        CloseHandle(pipe);
        return 1;
    }
    else
    {
        printf("Dado enviado com sucesso. Bytes escritos: %lu\n", bytesWritten);
    }

    // Verifica se a leitura do pipe foi bem-sucedida
    if (ReadFile(pipe, buffer, sizeof(buffer), &bytesRead, NULL))
    {
        if (bytesRead > 0)
        {
            buffer[bytesRead] = '\0';
            printf("Dado recebido: %s\n", buffer);
        }
        else
        {
            printf("Nenhum dado foi lido.\n");
        }
    }
    else
    {
        printf("Erro ao ler do pipe. Código do erro: %lu\n", GetLastError());
    }

    CloseHandle(pipe);

    return 0;
}
