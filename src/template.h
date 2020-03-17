#include "hashmap.h"
#include "vector.h"

char *read_file(char *filename);
char *template(char *tmpl, struct hashmap *ctx);

struct env;
struct env *env_new();
void env_free(struct env *env);
char *render(struct env *env, char *template_name, struct hashmap *ctx);