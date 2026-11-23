#ifndef T_HUFFMAN_H
#define T_HUFFMAN_H

#define ALPHABET_SIZE 256

typedef enum {

    HUFFMAN_OK,
    HUFFMAN_EMPTY_TABLE,
    HUFFMAN_NOT_CALCULATED,
    HUFFMAN_UNKNOWN_SYMBOL,
    HUFFMAN_INVALID_BITS,
    HUFFMAN_OUT_OF_MEMORY
} t_huffman_status;

typedef struct t_huffman t_huffman;

t_huffman *create_huffman(void);
void destroy_huffman(t_huffman *huffman);

void huffman_set_frequency(t_huffman *huffman, unsigned char simbolo, long frequencia);
void huffman_clear(t_huffman *huffman);

t_huffman_status huffman_calculate(t_huffman *huffman);
t_huffman_status huffman_print_table(const t_huffman *huffman);

t_huffman_status huffman_encode(const t_huffman *huffman, const char *texto, char **bits);
t_huffman_status huffman_decode(const t_huffman *huffman, const char *bits, char **texto);

int huffman_fixed_code_length(const t_huffman *huffman);

char *huffman_status_message(t_huffman_status status);

#endif
