# Análise de Proc. e Threads

Este projeto implementa um servidor multithreaded utilizando pool de threads para processar requisições de clientes. A comunicação entre o servidor e os clientes é feita via utilizando conceitos IPC, o servidor é capaz de processar dois tipos de dados: strings (convertendo-as para maiúsculas) e números (multiplicando-os por dois).

## Compilação

O projeto já contém um arquivo Makefile que compila automaticamente todos os arquivos necessários. Para compilar o projeto, execute o comando:

"make"

Este comando gerará os executáveis necessários, incluindo o servidor e os consumidores (clientes de string e de número).

## Executando o Servidor

Após a compilação, o servidor pode ser iniciado com o seguinte comando:

"./server_producer"

O servidor irá escutar em dois sockets Unix, um para strings e outro para números, aguardando conexões de clientes.

## Testando o Servidor

Existem duas formas de testar o servidor: conectar um cliente de string ou número, ou conectar múltiplos clientes simultaneamente usando o script "client_connection.sh"

1. Conectar um cliente de String ou Número
Para conectar um cliente de string, utilize o seguinte comando:

"./string_consumer"

Para conectar um cliente de número, utilize:

"./number_consumer"

Esses clientes enviarão dados ao servidor, que processará a solicitação e retornará a resposta apropriada.

## Testar Múltiplas Conexões Simultâneas

Para testar múltiplas conexões ao mesmo tempo, use o script "client_connection.sh". Por padrão, o script conecta vários clientes ao servidor simultaneamente. Para executá-lo, utilize:

```./client_connection.sh```