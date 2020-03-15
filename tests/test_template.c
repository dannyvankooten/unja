#include "test.h"
#include "template.h"

START_TESTS 

TEST(text_only) {
    char *input = "Hello world.";
    char *output = template(input, NULL);
    assert_str(output, "Hello world.");
    free(output);
}

TEST(expr_number) {
    char *input = "Hello {{ 5 }}.";
    char *output = template(input, NULL);
    assert_str(output, "Hello 5.");
    free(output);
}

TEST(expr_string) {
    char *input = "Hello {{ \"world\" }}.";
    char *output = template(input, NULL);
    assert_str(output, "Hello world.");
    free(output);
}

TEST(expr_symbol) {
    char *input = "Hello {{name}}.";
    struct hashmap *ctx = hashmap_new();
    hashmap_insert(ctx, "name", "world");
    char *output = template(input, ctx);
    assert_str(output, "Hello world.");
    hashmap_free(ctx);
    free(output);
}

TEST(expr_add) {
    struct {
        char *input;
        char *expected_output;
    } tests[] = {
        {"{{ 5 + 5 }}.", "10."},
        {"{{ 5 + foo }}.", "15."},
        {"{{ \"foo\" + \"bar\" }}", "foobar"},
        {"{{ \"Hello \" + name }}", "Hello Danny"},
    };

    struct hashmap *ctx = hashmap_new();
    hashmap_insert(ctx, "foo", "10");
    hashmap_insert(ctx, "name", "Danny");

    for (int i=0; i < ARRAY_SIZE(tests); i++) {
        char *output = template(tests[i].input, ctx);
        assert_str(output, tests[i].expected_output);
        free(output);
    }

    hashmap_free(ctx);
}

TEST(expr_subtract) {
    char *input = "Hello {{ 5 - 5 }}.";
    char *output = template(input, NULL);
    assert_str(output, "Hello 0.");
    free(output);
}

TEST(expr_divide) {
    char *input = "Hello {{ 5 / 5 }}.";
    char *output = template(input, NULL);
    assert_str(output, "Hello 1.");
    free(output);
}

TEST(expr_multiply) {
    char *input = "Hello {{ 5 * 5 }}.";
    char *output = template(input, NULL);
    assert_str(output, "Hello 25.");
    free(output);
}

TEST(expr_gt) {
    char *input = "Hello {{ 5 > 4 }}.";
    char *output = template(input, NULL);
    assert_str(output, "Hello 1.");
    free(output);

    input = "Hello {{ 5 > 6 }}.";
    output = template(input, NULL);
    assert_str(output, "Hello 0.");
    free(output);
}

TEST(expr_lt) {
    char *input = "Hello {{ 5 < 4 }}.";
    char *output = template(input, NULL);
    assert_str(output, "Hello 0.");
    free(output);

    input = "Hello {{ 4 < 5 }}.";
    output = template(input, NULL);
    assert_str(output, "Hello 1.");
    free(output);
}

TEST(expr_whitespace) {
    char *input = "Hello \n{{-name -}}\n.";
    struct hashmap *ctx = hashmap_new();
    hashmap_insert(ctx, "name", "world");
    char *output = template(input, ctx);
    assert_str(output, "Helloworld.");
    hashmap_free(ctx);
    free(output);
}

TEST(for_block) {
    char *input = "{% for n in numbers %}{{ n }}, {% endfor %}";
    struct hashmap *ctx = hashmap_new();

    struct vector *numbers = vector_new(3);
    vector_push(numbers, "1");
    vector_push(numbers, "2");
    vector_push(numbers, "3");
    hashmap_insert(ctx, "numbers", numbers);

    char *output = template(input, ctx);
    assert_str(output, "1, 2, 3, ");
    vector_free(numbers);
    hashmap_free(ctx);
    free(output);
}

TEST(var_dot_notation) {
    char *input = "Hello {{user.name}}!";
     struct hashmap *user = hashmap_new();
    hashmap_insert(user, "name", "Danny");

    struct hashmap *ctx = hashmap_new();
    hashmap_insert(ctx, "user", user);
    
    char *output = template(input, ctx);
    assert_str(output, "Hello Danny!");
    hashmap_free(ctx);
    free(output);
}

TEST(comments) {
    char *input = "Hello {# comment here #} world.";
    char *output = template(input, NULL);
    assert_str(output, "Hello  world.");
    free(output);
}

END_TESTS 
