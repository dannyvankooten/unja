#include "hashmap.h"
#include "vector.h"


struct env;
struct env *env_new();
void env_free(struct env *env);
char *template(struct env *env, char *template_name, struct hashmap *ctx);
char *template_string(char *tmpl, struct hashmap *ctx);
char *read_file(char *filename);