#include "template.h"
#include <stdlib.h>

int main() {
    char *input = read_file("index.tpl");

    struct hashmap *ctx = hashmap_new();
    hashmap_insert(ctx, "title", "Hello world");

    struct post home = { 
        .title = "Homepage",
        .tags = {
            "Tag 1", "Tag 2"
        }
    };
    hashmap_insert(ctx, "home", &home);

    struct post posts[] = {
        { .title = "Post 1", .tags = { "p1t1" } },
        { .title = "Post 2", .tags = { "p2t1" } },
    };
    hashmap_insert(ctx, "posts", &posts);

    template(input, ctx);
    hashmap_free(ctx);
    free(input);
}