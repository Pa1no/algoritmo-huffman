# Algoritmo de Huffman em C

Trabalho Prático 06 de Estrutura de Dados

Programa que usa o algoritmo de Huffman para calcular uma codificação mínima a partir de uma tabela de frequências, permitindo codificar e decodificar sequências.

## Integrantes

- Eduardo Paino
- Bernardo Kapp

## Estrutura do projeto

```
.
├── main.c               # Interpretador de comandos
├── t_huffman.c          # TAD Huffman: tabela de frequências, árvore, códigos, cod/dec
├── t_huffman.h
├── t_priority_queue.c   # TAD Fila de Prioridade (heap mínimo genérico)
├── t_priority_queue.h
├── tests/               # Entradas (.in) e saídas esperadas (.out)
├── makefile
└── README.md
```

## Compilação e execução

```bash
make            # compila e gera o executável ./huffman
make run        # compila e executa (digitar os comandos no terminal)
make test       # roda todos os casos da pasta tests/ e compara com a saída esperada
make valgrind   # executa com o valgrind para verificar vazamentos de memória
make clean      # remove os arquivos objeto (.o)
make fclean     # remove os .o e o executável
make re         # limpa tudo e recompila
```

## Comandos

| Comando | Descrição |
|---|---|
| `add E freq` | informa a frequência do caractere `E` (se já existia, substitui) |
| `calc` | calcula os códigos de Huffman dos caracteres informados |
| `print` | imprime a tabela de códigos |
| `cod str` | mostra os bits de `str` e as taxas de compactação |
| `dec bits` | mostra o texto correspondente à sequência de bits |
| `clear` | zera a tabela de frequência |
| `exit` | sai do programa |

### Exemplo

```
->add A 3
->add C 2
->add B 1
->add D 1
->calc
->print
A com frequencia 3 codificado como 0
B com frequencia 1 codificado como 100
C com frequencia 2 codificado como 11
D com frequencia 1 codificado como 101
->cod ABACCDA
0100011111010 23.21% 92.86%
->dec 0100011111010
ABACCDA
->exit
```

## Como funciona

### Construção da árvore (`calc`)

1. Cada caractere com frequência maior que zero vira uma folha e é inserido em uma **fila de prioridade** (heap mínimo).
2. Enquanto houver mais de um nó na fila, os **dois de menor frequência** são retirados e unidos em um novo nó, cuja frequência é a soma dos dois. O primeiro retirado vira o filho da esquerda (bit `0`) e o segundo o da direita (bit `1`).
3. O nó que sobra é a raiz. Os códigos são gerados percorrendo a árvore até cada folha.

**Desempate:** quando dois nós têm a mesma frequência, vem primeiro o que contém o menor caractere (ordem ASCII). Assim o resultado é sempre o mesmo, independente da ordem em que os `add` foram feitos, e bate com o exemplo do enunciado.

### Taxas de compactação (`cod`)

Para `cod ABACCDA` são gerados 13 bits:

- **em relação a 8 bits por caractere:** `13 / (7 × 8) = 23.21%`
- **em relação ao mínimo de bits de um código de tamanho fixo:** com 4 caracteres na tabela bastam `⌈log2(4)⌉ = 2` bits, então `13 / (7 × 2) = 92.86%`

Quanto menor a porcentagem, maior a compactação.

### Decodificação (`dec`)

Os bits são lidos um a um, descendo na árvore (`0` esquerda, `1` direita). Ao chegar em uma folha o caractere é escrito e volta-se para a raiz. A sequência é inválida se tiver algo diferente de `0`/`1` ou se terminar no meio de um código.

## Decisões de implementação

- Depois de um `add` ou `clear` os códigos antigos são descartados; é preciso rodar `calc` de novo antes de `print`, `cod` ou `dec`.
- `add E 0` remove o caractere do cálculo. Frequências negativas são recusadas.
- Com um único caractere na tabela a raiz já é uma folha; ele recebe o código `0`.
- Erros (comando desconhecido, caractere sem código, bits inválidos, etc.) são informados com uma mensagem começando com `erro:`.
- A fila de prioridade é genérica (guarda `void *` e recebe uma função de comparação), então poderia ser reaproveitada em outros trabalhos.
