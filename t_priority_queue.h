#ifndef T_PRIORITY_QUEUE_H
#define T_PRIORITY_QUEUE_H

typedef int (*t_compare_function)(const void *a, const void *b);

typedef struct t_priority_queue t_priority_queue;

t_priority_queue *create_priority_queue(int capacidade, t_compare_function comparar);
void destroy_priority_queue(t_priority_queue *fila);
int priority_queue_size(const t_priority_queue *fila);
int priority_queue_insert(t_priority_queue *fila, void *item);
void *priority_queue_extract_min(t_priority_queue *fila);

#endif
