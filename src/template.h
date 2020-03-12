#include "hashmap.h"

char *read_file(char *filename);
void template(char *tmpl, struct hashmap *ctx);

struct post {
    char title[64];
    char tags[8][32];
};