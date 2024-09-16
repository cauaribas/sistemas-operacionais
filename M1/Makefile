# Nome do compilador
CC = gcc

# Flags de compilação
CFLAGS = -Wall -pthread

# Executáveis
EXEC_PRODUCER = named_pipe_producer
EXEC_STRING = string_consumer
EXEC_NUMBER = number_consumer

# Regra padrão para compilar todos os executáveis
all: $(EXEC_PRODUCER) $(EXEC_STRING) $(EXEC_NUMBER)

# Regra para compilar o produtor
$(EXEC_PRODUCER): named_pipe_producer.c
	$(CC) $(CFLAGS) -o $@ $<

# Regra para compilar o cliente de strings
$(EXEC_STRING): string_consumer.c
	$(CC) $(CFLAGS) -o $@ $<

# Regra para compilar o cliente de números
$(EXEC_NUMBER): number_consumer.c
	$(CC) $(CFLAGS) -o $@ $<

# Limpar os arquivos compilados
clean:
	rm -f $(EXEC_PRODUCER) $(EXEC_STRING) $(EXEC_NUMBER)

# Phony targets
.PHONY: all clean
