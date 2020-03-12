#include <stdlib.h>

struct vector {
    void **values;
    int size;
    int cap;
};

struct vector* vector_new(int cap);
int vector_push(struct vector *vec, void *value);
void vector_free(struct vector *vec);