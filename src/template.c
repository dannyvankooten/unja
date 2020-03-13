#include <stdio.h>
#include <stdlib.h>
#include "vendor/mpc.h"
#include "template.h"

char *read_file(char *filename) {
    char *input = malloc(BUFSIZ);
    unsigned int size = 0;

    FILE *f = fopen(filename, "r");
    if (!f) {
        printf("Could not open \"%s\" for reading", filename);
        exit(1);
    }

    unsigned int read = 0;
    while ( (read = fread(input, 1, BUFSIZ, f)) > 0) {
        size += read;
        input = realloc(input, size + BUFSIZ);
    }

    fclose(f);

    input[size] = '\0';
    return input;
}

char *trim_trailing_whitespace(char *str) {
    for (int i=strlen(str)-1; isspace(str[i]); i--) {
        str[i] = '\0';
    }
    return str;
}

char *trim_leading_whitespace(char *str) {
    while (isspace(*str)) {
        str++;
    }

    return str;
}

int eval(char *dest, mpc_ast_t* t, struct hashmap *ctx) {
    static int trim_whitespace = 0;

    if (strstr(t->tag, "content|expression|")) {
        // maybe eat whitespace going backward
        if (strstr(t->children[1]->contents, "-")) {
            dest = trim_trailing_whitespace(dest);
        }

        // set flag for next eval() to trim leading whitespace from text
        if (strstr(t->children[3]->contents, "-")) {
            trim_whitespace = 1;
        }

        char *value = NULL;
        char *key = t->children[2]->contents;
        value = hashmap_resolve(ctx, key);
        
        // TODO: Handle unexisting keys
        if  (value == NULL) {
            return 1;
        }
        strcat(dest, value);
        return 0;
    }

    if (strstr(t->tag, "content|for")) {
        char *tmp_key = t->children[2]->contents;
        char *iterator_key = t->children[4]->contents;
        struct vector *list = hashmap_resolve(ctx, iterator_key);
        for (int i=0; i < list->size; i++) {
            hashmap_insert(ctx, tmp_key, list->values[i]);
            eval(dest, t->children[6], ctx);
        }
        return 0;
    }

    if (strstr(t->tag, "content|text")) {
        char *str = t->contents;
        if (trim_whitespace) {
            str = trim_leading_whitespace(str);
            trim_whitespace = 0;
        }

        strcat(dest, str);
        return 0;
    }

    for (int i=0; i < t->children_num; i++) {
        eval(dest, t->children[i], ctx);
    }
    
    return 0;
}

mpc_parser_t *parser_init() {
    mpc_parser_t *symbol = mpc_new("symbol");
    mpc_parser_t *text = mpc_new("text");
    mpc_parser_t *expression = mpc_new("expression");
    mpc_parser_t *statement = mpc_new("statement_open");
    mpc_parser_t *statement_open = mpc_new("statement_open");
    mpc_parser_t *statement_close = mpc_new("statement_close");
    mpc_parser_t *comment = mpc_new("comment");
    mpc_parser_t *for_statement = mpc_new("for");
    mpc_parser_t *body = mpc_new("body");
    mpc_parser_t *content = mpc_new("content");
    mpc_parser_t *template = mpc_new("template");
    mpca_lang(MPCA_LANG_WHITESPACE_SENSITIVE,
        " symbol    : /[a-zA-Z_.]+/ ;"
        " text      : /[^{][^{%#]*/ ;"
        " expression : \"{{\" /-? */ <symbol> / *-?/ \"}}\" ;"
        " comment : \"{#\" /[^#][^#}]*/ \"#}\" ;"
        " statement_open: \"{%\" /-? */;"
        " statement_close: / *-?/ \"%}\";"
        " for       : <statement_open> \"for \" <symbol> \" in \" <symbol> <statement_close> <body> <statement_open> \"endfor\" <statement_close> ;"
        " content   : <expression> | <for> | <text> | <comment>;"
        " body      : <content>* ;"
        " template  : /^/ <body> /$/ ;",
        symbol, 
        expression, 
        statement_open, 
        statement_close, 
        comment,
        for_statement, 
        text, 
        content, 
        body, 
        template, 
        NULL);
    return template;
}

char *template(char *tmpl, struct hashmap *ctx) {
    mpc_parser_t *parser = parser_init();
    mpc_result_t r;

    if (!mpc_parse("input", tmpl, parser, &r)) {
        mpc_err_print(r.error);
        mpc_err_delete(r.error);
        return NULL;
    }

    #if DEBUG
    printf("Template: %s\n", tmpl);
    printf("AST: ");
    mpc_ast_print(r.output);
    printf("\n");
    #endif
        
    // FIXME: Allocate precisely
    char *output = malloc(strlen(tmpl) * 2);
    output[0] = '\0';

    eval(output, r.output, ctx);
    mpc_ast_delete(r.output);

    return output;
}

