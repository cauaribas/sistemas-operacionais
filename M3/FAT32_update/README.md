# Instruções de Compilação

### Linux

Este projeto foi feito para Linux somente. Usuários de Windows devem usar WSL.

O sistema deve ter um compilador GCC 11+, com suporte à C11, e GNU Make.

Para compilar, rode:

```
$ make resetimg obese32
```

Para executar, rode:
```
$ ./obese32 <COMANDO> [ARGUMENTOS] <DISCO>
```

### Windows

Veja [Como instalar o Linux no Windows com o WSL](https://learn.microsoft.com/pt-br/windows/wsl/install), por Microsoft.

# Implementação

Os seguintes comandos foram implementados:

1. Listar   -- ls
2. Mover    -- mv
3. Remover  -- rm
4. Copiar   -- cp
5. Imprimir -- cat

# Exemplos

Por exemplo, para listar os arquivos:

```
$ ./obese32 ls disk.img
```

Para mover um arquivo:

```
$ ./obese32 mv teste.txt other.exe disk.img
```

Para remover um arquivo:

```
$ ./obese32 rm texto2.txt disk.img
```

Para copiar um arquivo:

```
$ ./obese32 cp texto2.txt novo.txt disk.img
```

Para imprimir um arquivo:

```
$ ./obese32 cat teste.txt disk.img
```

# Guia Documentação

Veja na pasta `docs/` os arquivos `FAT16.md`, `API.md` e `Guia.md`. O código em
sí também foi altamente documentado.

O arquivo para implementação das funções é o `source/commands.c`.