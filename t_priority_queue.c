#include <stdlib.h>

#include "t_priority_queue.h"

struct t_priority_queue {

    void **itens;
    int tamanho;
    int capacidade;
    t_compare_function comparar;
};

t_priority_queue *create_priority_queue(int capacidade, t_compare_function comparar) {

    if(capacidade <= 0) {

        return NULL;
    }

    t_priority_queue *fila = malloc(sizeof(t_priority_queue));

    if(fila == NULL) {

        return NULL;
    }

    fila->itens = malloc(capacidade * sizeof(void *));

    if(fila->itens == NULL) {

        free(fila);
        return NULL;
    }

    fila->tamanho = 0;
    fila->capacidade = capacidade;
    fila->comparar = comparar;

    return fila;
}

void destroy_priority_queue(t_priority_queue *fila) {

    if(fila == NULL) {

        return;
    }

    free(fila->itens);
    free(fila);
}

int priority_queue_size(const t_priority_queue *fila) {

    return fila->tamanho;
}

int parent_of(int indice) {

    return (indice - 1) / 2;
}

int is_smaller(const t_priority_queue *fila, int i, int j) {

    return fila->comparar(fila->itens[i], fila->itens[j]) < 0;
}

void swap_items(t_priority_queue *fila, int i, int j) {

    void *temporario = fila->itens[i];
    fila->itens[i] = fila->itens[j];
    fila->itens[j] = temporario;
}

void sift_up(t_priority_queue *fila, int indice) {

    while(indice > 0 && is_smaller(fila, indice, parent_of(indice))) {

        swap_items(fila, indice, parent_of(indice));
        indice = parent_of(indice);
    }
}

int smallest_child(const t_priority_queue *fila, int indice) {

    int esquerdo = 2 * indice + 1;
    int direito = esquerdo + 1;

    if(direito < fila->tamanho && is_smaller(fila, direito, esquerdo)) {

        return direito;
    }

    return esquerdo;
}

void sift_down(t_priority_queue *fila, int indice) {

    while(2 * indice + 1 < fila->tamanho) {

        int filho = smallest_child(fila, indice);

        if(!is_smaller(fila, filho, indice)) {

            return;
        }

        swap_items(fila, indice, filho);
        indice = filho;
    }
}

int priority_queue_insert(t_priority_queue *fila, void *item) {

    if(fila->tamanho == fila->capacidade) {

        return 0;
    }

    fila->itens[fila->tamanho] = item;
    fila->tamanho++;
    sift_up(fila, fila->tamanho - 1);

    return 1;
}

void *priority_queue_extract_min(t_priority_queue *fila) {

    if(fila->tamanho == 0) {

        return NULL;
    }

    void *menor = fila->itens[0];

    fila->tamanho--;
    fila->itens[0] = fila->itens[fila->tamanho];
    sift_down(fila, 0);

    return menor;
}
