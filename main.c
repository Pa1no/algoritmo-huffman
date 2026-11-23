/*
    Integrantes do grupo: Eduardo Paino e Bernardo Kapp
*/

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "t_huffman.h"

#define INITIAL_LINE_SIZE 128
#define BITS_PER_BYTE 8

char *read_line(void) {

    int capacidade = INITIAL_LINE_SIZE;
    int tamanho = 0;
    int caractere;
    char *linha = malloc(capacidade);

    if(linha == NULL) {

        return NULL;
    }

    while((caractere = getchar()) != EOF && caractere != '\n') {

        if(tamanho + 1 == capacidade) {

            char *linha_maior = realloc(linha, capacidade * 2);

            if(linha_maior == NULL) {

                free(linha);
                return NULL;
            }

            linha = linha_maior;
            capacidade *= 2;
        }

        linha[tamanho] = caractere;
        tamanho++;
    }

    if(caractere == EOF && tamanho == 0) {

        free(linha);
        return NULL;
    }

    linha[tamanho] = '\0';

    return linha;
}

char *trim(char *texto) {

    while(isspace((unsigned char) *texto)) {

        texto++;
    }

    int tamanho = strlen(texto);

    while(tamanho > 0 && isspace((unsigned char) texto[tamanho - 1])) {

        texto[tamanho - 1] = '\0';
        tamanho--;
    }

    return texto;
}

char *split_argument(char *linha) {

    char *separador = linha;

    while(*separador != '\0' && !isspace((unsigned char) *separador)) {

        separador++;
    }

    if(*separador == '\0') {

        return separador;
    }

    *separador = '\0';

    return trim(separador + 1);
}

void print_error(t_huffman_status status) {

    printf("erro: %s\n", huffman_status_message(status));
}

int parse_frequency(const char *texto, long *frequencia) {

    char *fim;

    errno = 0;
    *frequencia = strtol(texto, &fim, 10);

    return errno == 0 && fim != texto && *trim(fim) == '\0' && *frequencia >= 0;
}

void execute_add(t_huffman *huffman, char *argumento) {

    long frequencia;

    if(argumento[0] == '\0' || !isspace((unsigned char) argumento[1])) {

        printf("erro: use add <caractere> <frequencia>\n");
        return;
    }

    if(!parse_frequency(argumento + 2, &frequencia)) {

        printf("erro: frequencia invalida\n");
        return;
    }

    huffman_set_frequency(huffman, (unsigned char) argumento[0], frequencia);
}

void execute_calc(t_huffman *huffman) {

    t_huffman_status status = huffman_calculate(huffman);

    if(status != HUFFMAN_OK) {

        print_error(status);
    }
}

void execute_print(const t_huffman *huffman) {

    t_huffman_status status = huffman_print_table(huffman);

    if(status != HUFFMAN_OK) {

        print_error(status);
    }
}

double compression_rate(size_t bits_codificados, size_t bits_originais) {

    return 100.0 * bits_codificados / bits_originais;
}

void execute_cod(const t_huffman *huffman, const char *texto) {

    char *bits;

    if(*texto == '\0') {

        printf("erro: use cod <string>\n");
        return;
    }

    t_huffman_status status = huffman_encode(huffman, texto, &bits);

    if(status != HUFFMAN_OK) {

        print_error(status);
        return;
    }

    size_t tamanho_texto = strlen(texto);
    size_t bits_codificados = strlen(bits);
    size_t tamanho_fixo = huffman_fixed_code_length(huffman);

    printf("%s %.2f%% %.2f%%\n", bits,
           compression_rate(bits_codificados, tamanho_texto * BITS_PER_BYTE),
           compression_rate(bits_codificados, tamanho_texto * tamanho_fixo));

    free(bits);
}

void execute_dec(const t_huffman *huffman, const char *bits) {

    char *texto;

    if(*bits == '\0') {

        printf("erro: use dec <bits>\n");
        return;
    }

    t_huffman_status status = huffman_decode(huffman, bits, &texto);

    if(status != HUFFMAN_OK) {

        print_error(status);
        return;
    }

    printf("%s\n", texto);
    free(texto);
}

int execute_command(t_huffman *huffman, char *linha) {

    char *comando = trim(linha);
    char *argumento = split_argument(comando);

    if(strcmp(comando, "add") == 0)        execute_add(huffman, argumento);
    else if(strcmp(comando, "calc") == 0)  execute_calc(huffman);
    else if(strcmp(comando, "print") == 0) execute_print(huffman);
    else if(strcmp(comando, "cod") == 0)   execute_cod(huffman, argumento);
    else if(strcmp(comando, "dec") == 0)   execute_dec(huffman, argumento);
    else if(strcmp(comando, "clear") == 0) huffman_clear(huffman);
    else if(strcmp(comando, "exit") == 0)  return 0;
    else if(*comando != '\0')              printf("erro: comando desconhecido\n");

    return 1;
}

void print_prompt(void) {

    printf("->");
    fflush(stdout);
}

int main() {

    t_huffman *huffman = create_huffman();

    if(huffman == NULL) {

        fprintf(stderr, "Erro ao alocar memoria.\n");
        return 1;
    }

    int executando = 1;
    char *linha;

    print_prompt();

    while(executando && (linha = read_line()) != NULL) {

        executando = execute_command(huffman, linha);
        free(linha);

        if(executando) {

            print_prompt();
        }
    }

    destroy_huffman(huffman);

    return 0;
}
