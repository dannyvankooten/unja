#include "test.h"
#include "template.h"

START_TESTS 

TEST(text_only) {
    char *input = "Hello world.";
    char *output = template(input, NULL);
    assert_str(output, "Hello world.");
    free(output);
}

TEST(text_with_var) {
    char *input = "Hello {{name}}.";
    struct hashmap *ctx = hashmap_new();
    hashmap_insert(ctx, "name", "world");
    char *output = template(input, ctx);
    assert_str(output, "Hello world.");
    hashmap_free(ctx);
    free(output);
}

TEST(text_multiline) {
    char *input = "Hello {{name}}.\nL2";
    struct hashmap *ctx = hashmap_new();
    hashmap_insert(ctx, "name", "world");
    char *output = template(input, ctx);
    assert_str(output, "Hello world.\nL2");
    hashmap_free(ctx);
    free(output);
}




END_TESTS 