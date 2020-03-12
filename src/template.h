#include "hashmap.h"

char *read_file(char *filename);
char *template(char *tmpl, struct hashmap *ctx);

struct post {
    char title[64];
    char tags[8][32];
};

struct list {
    void **values;
    int size;
    int cap;
};