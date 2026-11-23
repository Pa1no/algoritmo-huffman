#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "t_huffman.h"
#include "t_priority_queue.h"

typedef struct {

    unsigned char simbolo;
    unsigned char menor_simbolo;
    long frequencia;
    struct t_huffman_node *esquerda;
    struct t_huffman_node *direita;
} t_huffman_node;

typedef struct {

    long frequencias[ALPHABET_SIZE];
    char *codigos[ALPHABET_SIZE];
    t_huffman_node *raiz;
    int quantidade_simbolos;
} t_huffman;

t_huffman_node *create_leaf(unsigned char simbolo, long frequencia) {

    t_huffman_node *no = malloc(sizeof(t_huffman_node));

    if(no == NULL) {

        return NULL;
    }

    no->simbolo = simbolo;
    no->menor_simbolo = simbolo;
    no->frequencia = frequencia;
    no->esquerda = NULL;
    no->direita = NULL;

    return no;
}

t_huffman_node *create_parent(t_huffman_node *esquerda, t_huffman_node *direita) {

    t_huffman_node *no = create_leaf(esquerda->menor_simbolo, esquerda->frequencia + direita->frequencia);

    if(no == NULL) {

        return NULL;
    }

    if(direita->menor_simbolo < no->menor_simbolo) {

        no->menor_simbolo = direita->menor_simbolo;
    }

    no->esquerda = esquerda;
    no->direita = direita;

    return no;
}

int is_leaf(const t_huffman_node *no) {

    return no->esquerda == NULL && no->direita == NULL;
}

void destroy_nodes(t_huffman_node *no) {

    if(no == NULL) {

        return;
    }

    destroy_nodes(no->esquerda);
    destroy_nodes(no->direita);
    free(no);
}

int compare_nodes(const void *a, const void *b) {

    const t_huffman_node *primeiro = a;
    const t_huffman_node *segundo = b;

    if(primeiro->frequencia != segundo->frequencia) {

        return primeiro->frequencia < segundo->frequencia ? -1 : 1;
    }

    return (int) primeiro->menor_simbolo - (int) segundo->menor_simbolo;
}

t_huffman *create_huffman(void) {

    t_huffman *huffman = malloc(sizeof(t_huffman));

    if(huffman == NULL) {

        return NULL;
    }

    for(int i = 0; i < ALPHABET_SIZE; i++) {

        huffman->frequencias[i] = 0;
        huffman->codigos[i] = NULL;
    }

    huffman->raiz = NULL;
    huffman->quantidade_simbolos = 0;

    return huffman;
}

void discard_codes(t_huffman *huffman) {

    for(int i = 0; i < ALPHABET_SIZE; i++) {

        free(huffman->codigos[i]);
        huffman->codigos[i] = NULL;
    }

    destroy_nodes(huffman->raiz);
    huffman->raiz = NULL;
    huffman->quantidade_simbolos = 0;
}

void destroy_huffman(t_huffman *huffman) {

    if(huffman == NULL) {

        return;
    }

    discard_codes(huffman);
    free(huffman);
}

void huffman_set_frequency(t_huffman *huffman, unsigned char simbolo, long frequencia) {

    huffman->frequencias[simbolo] = frequencia;
    discard_codes(huffman);
}

void huffman_clear(t_huffman *huffman) {

    for(int i = 0; i < ALPHABET_SIZE; i++) {

        huffman->frequencias[i] = 0;
    }

    discard_codes(huffman);
}

int is_calculated(const t_huffman *huffman) {

    return huffman->raiz != NULL;
}

t_huffman_status insert_leaves(t_huffman *huffman, t_priority_queue *fila) {

    for(int simbolo = 0; simbolo < ALPHABET_SIZE; simbolo++) {

        if(huffman->frequencias[simbolo] <= 0) {

            continue;
        }

        t_huffman_node *folha = create_leaf((unsigned char) simbolo, huffman->frequencias[simbolo]);

        if(folha == NULL) {

            return HUFFMAN_OUT_OF_MEMORY;
        }

        priority_queue_insert(fila, folha);
        huffman->quantidade_simbolos++;
    }

    return huffman->quantidade_simbolos > 0 ? HUFFMAN_OK : HUFFMAN_EMPTY_TABLE;
}

t_huffman_status merge_nodes(t_priority_queue *fila) {

    while(priority_queue_size(fila) > 1) {

        t_huffman_node *esquerda = priority_queue_extract_min(fila);
        t_huffman_node *direita = priority_queue_extract_min(fila);
        t_huffman_node *pai = create_parent(esquerda, direita);

        if(pai == NULL) {

            destroy_nodes(esquerda);
            destroy_nodes(direita);
            return HUFFMAN_OUT_OF_MEMORY;
        }

        priority_queue_insert(fila, pai);
    }

    return HUFFMAN_OK;
}

void destroy_remaining_nodes(t_priority_queue *fila) {

    t_huffman_node *no;

    while((no = priority_queue_extract_min(fila)) != NULL) {

        destroy_nodes(no);
    }
}

char *copy_string(const char *origem) {

    char *copia = malloc(strlen(origem) + 1);

    if(copia != NULL) {

        strcpy(copia, origem);
    }

    return copia;
}

int store_codes(t_huffman *huffman, const t_huffman_node *no, char *caminho, int profundidade) {

    if(is_leaf(no)) {

        if(profundidade == 0) {

            caminho[profundidade] = '0';
            profundidade++;
        }

        caminho[profundidade] = '\0';
        huffman->codigos[no->simbolo] = copy_string(caminho);

        return huffman->codigos[no->simbolo] != NULL;
    }

    caminho[profundidade] = '0';

    if(!store_codes(huffman, no->esquerda, caminho, profundidade + 1)) {

        return 0;
    }

    caminho[profundidade] = '1';

    return store_codes(huffman, no->direita, caminho, profundidade + 1);
}

t_huffman_status huffman_calculate(t_huffman *huffman) {

    discard_codes(huffman);

    t_priority_queue *fila = create_priority_queue(ALPHABET_SIZE, compare_nodes);

    if(fila == NULL) {

        return HUFFMAN_OUT_OF_MEMORY;
    }

    t_huffman_status status = insert_leaves(huffman, fila);

    if(status == HUFFMAN_OK) {

        status = merge_nodes(fila);
    }

    if(status != HUFFMAN_OK) {

        destroy_remaining_nodes(fila);
        destroy_priority_queue(fila);
        huffman->quantidade_simbolos = 0;
        return status;
    }

    huffman->raiz = priority_queue_extract_min(fila);
    destroy_priority_queue(fila);

    char caminho[ALPHABET_SIZE + 1];

    if(!store_codes(huffman, huffman->raiz, caminho, 0)) {

        discard_codes(huffman);
        return HUFFMAN_OUT_OF_MEMORY;
    }

    return HUFFMAN_OK;
}

t_huffman_status huffman_print_table(const t_huffman *huffman) {

    if(!is_calculated(huffman)) {

        return HUFFMAN_NOT_CALCULATED;
    }

    for(int simbolo = 0; simbolo < ALPHABET_SIZE; simbolo++) {

        if(huffman->codigos[simbolo] != NULL) {

            printf("%c com frequencia %ld codificado como %s\n",
                   simbolo, huffman->frequencias[simbolo], huffman->codigos[simbolo]);
        }
    }

    return HUFFMAN_OK;
}

char *code_of(const t_huffman *huffman, char simbolo) {

    return huffman->codigos[(unsigned char) simbolo];
}

t_huffman_status huffman_encode(const t_huffman *huffman, const char *texto, char **bits) {

    if(!is_calculated(huffman)) {

        return HUFFMAN_NOT_CALCULATED;
    }

    size_t tamanho_total = 0;

    for(int i = 0; texto[i] != '\0'; i++) {

        if(code_of(huffman, texto[i]) == NULL) {

            return HUFFMAN_UNKNOWN_SYMBOL;
        }

        tamanho_total += strlen(code_of(huffman, texto[i]));
    }

    *bits = malloc(tamanho_total + 1);

    if(*bits == NULL) {

        return HUFFMAN_OUT_OF_MEMORY;
    }

    char *posicao = *bits;

    for(int i = 0; texto[i] != '\0'; i++) {

        const char *codigo = code_of(huffman, texto[i]);
        size_t tamanho_codigo = strlen(codigo);

        memcpy(posicao, codigo, tamanho_codigo);
        posicao += tamanho_codigo;
    }

    *posicao = '\0';

    return HUFFMAN_OK;
}

t_huffman_node *follow_bit(const t_huffman_node *no, char bit) {

    if(bit == '0') {

        return no->esquerda;
    }

    if(bit == '1') {

        return no->direita;
    }

    return NULL;
}

int decode_single_symbol(const t_huffman_node *raiz, const char *bits, char *texto) {

    int tamanho = 0;

    for(int i = 0; bits[i] != '\0'; i++) {

        if(bits[i] != '0') {

            return 0;
        }

        texto[tamanho] = raiz->simbolo;
        tamanho++;
    }

    texto[tamanho] = '\0';

    return 1;
}

int decode_bits(const t_huffman_node *raiz, const char *bits, char *texto) {

    const t_huffman_node *atual = raiz;
    int tamanho = 0;

    for(int i = 0; bits[i] != '\0'; i++) {

        atual = follow_bit(atual, bits[i]);

        if(atual == NULL) {

            return 0;
        }

        if(is_leaf(atual)) {

            texto[tamanho] = atual->simbolo;
            tamanho++;
            atual = raiz;
        }
    }

    texto[tamanho] = '\0';

    return atual == raiz;
}

t_huffman_status huffman_decode(const t_huffman *huffman, const char *bits, char **texto) {

    if(!is_calculated(huffman)) {

        return HUFFMAN_NOT_CALCULATED;
    }

    *texto = malloc(strlen(bits) + 1);

    if(*texto == NULL) {

        return HUFFMAN_OUT_OF_MEMORY;
    }

    int sucesso;

    if(is_leaf(huffman->raiz)) {

        sucesso = decode_single_symbol(huffman->raiz, bits, *texto);
    }
    else {

        sucesso = decode_bits(huffman->raiz, bits, *texto);
    }

    if(!sucesso) {

        free(*texto);
        *texto = NULL;
        return HUFFMAN_INVALID_BITS;
    }

    return HUFFMAN_OK;
}

int huffman_fixed_code_length(const t_huffman *huffman) {

    int bits = 1;

    while((1 << bits) < huffman->quantidade_simbolos) {

        bits++;
    }

    return bits;
}

char *huffman_status_message(t_huffman_status status) {

    switch(status) {

        case HUFFMAN_OK:             return "ok";
        case HUFFMAN_EMPTY_TABLE:    return "tabela de frequencia vazia";
        case HUFFMAN_NOT_CALCULATED: return "codigos nao calculados, use o comando calc";
        case HUFFMAN_UNKNOWN_SYMBOL: return "a string possui caractere sem codigo na tabela";
        case HUFFMAN_INVALID_BITS:   return "sequencia de bits invalida";
        case HUFFMAN_OUT_OF_MEMORY:  return "memoria insuficiente";
    }

    return "erro desconhecido";
}
