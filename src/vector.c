#include <stdlib.h>
#include "vector.h"

/* create a new vector of the given capacity */
struct vector* vector_new(int cap) {
    struct vector *l = malloc(sizeof *l);
    l->size = 0;
    l->cap = cap;
    l->values = malloc(l->cap * sizeof *l->values);
    return l;
}

/* push a new value to the end of the vector's memory */
int vector_push(struct vector *vec, void *value) {
    vec->values[vec->size++] = value;
    return vec->size - 1;
}

/* free vector related memory */
void vector_free(struct vector *l) {
    free(l->values);
    free(l);
}