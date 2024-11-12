# Nome do compilador
CC = gcc

# Flags de compilação específicas para Windows
CFLAGS = -Wall -lws2_32

# Executáveis
EXEC_PRODUCER = server_producer.exe
EXEC_STRING = string_consumer.exe
EXEC_NUMBER = number_consumer.exe

# Regra padrão para compilar todos os executáveis
all: $(EXEC_PRODUCER) $(EXEC_STRING) $(EXEC_NUMBER)

# Regra para compilar o produtor
$(EXEC_PRODUCER): named_pipe_server_producer.c
	$(CC) $(CFLAGS) -o $@ $<

# Regra para compilar o cliente de strings
$(EXEC_STRING): named_pipe_string_consumer.c
	$(CC) $(CFLAGS) -o $@ $<

# Regra para compilar o cliente de números
$(EXEC_NUMBER): named_pipe_number_consumer.c
	$(CC) $(CFLAGS) -o $@ $<

# Limpar os arquivos compilados
clean:
	del /Q $(EXEC_PRODUCER) $(EXEC_STRING) $(EXEC_NUMBER)

# Phony targets
.PHONY: all clean
