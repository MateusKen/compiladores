# Compilador Pascal+-

Front-end de um compilador para a linguagem Pascal+- (um subconjunto de Pascal), escrito em C. Faz a análise léxica e a análise sintática descendente recursiva (ASDR) de um programa-fonte e aponta o primeiro erro encontrado.

Projeto da disciplina de Compiladores (Ciência da Computação, Universidade Presbiteriana Mackenzie, 2024/2).

## O que ele reconhece

- **Palavras reservadas:** `program`, `begin`, `end`, `if`, `elif`, `for`, `of`, `to`, `read`, `write`, `set`, `integer`, `boolean`, `true`, `false`, `and`, `or`, `not`
- **Identificadores** com até 15 caracteres e **números binários** (prefixo `0b`)
- **Operadores** aritméticos (`+ - * /`) e relacionais (`< <= = /= > >=`)
- **Comentários** de linha (`#`) e de bloco (`{- ... -}`)
- **Pontuação:** `; : , . ( )`

## Como compilar e executar

```bash
cd projeto1
```

```bash
gcc -g -Og -Wall compilador1.c -o compilador1
```

```bash
./compilador1 arquivo.txt
```

`arquivo.txt` é o programa-fonte em Pascal+- a ser analisado.

## Estrutura

```
projeto1/
├── compilador1.c                    # analisador léxico + sintático
├── arquivo.txt                      # programa de exemplo
└── 2024_2_trab1_lex_asdr_v3.pdf     # enunciado do trabalho
```

## Autor

Mateus Kenzo Iochimoto
