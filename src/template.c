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

int eval(char *dest, mpc_ast_t* t, struct hashmap *ctx) {
    if (strstr(t->tag, "content|var")) {
        char *value = NULL;

        // TODO: Optimize this so we can use it for 1..n keys
        if (t->children[1]->children_num == 0) {
            char *key = t->children[1]->contents;
            value = hashmap_get(ctx, key);
        } else {
            char *key = t->children[1]->children[0]->contents;
            ctx = hashmap_get(ctx, key);
            if (ctx == NULL) {
                // TODO: Handle unexisting keys
                return 1;
            }

            char *subkey = t->children[1]->children[2]->contents;
            value = hashmap_get(ctx, subkey);
        }

        // TODO: Handle unexisting keys
        if  (value == NULL) {
            return 1;
        }
        strcat(dest, value);
        return 0;
    }

    if (strstr(t->tag, "content|for")) {
        char *tmp_key = t->children[2]->contents;
        // TODO: Handle dot notation here.
        char *iterator_key = t->children[4]->contents;
        struct vector *list = hashmap_get(ctx, iterator_key);
        for (int i=0; i < list->size; i++) {
            hashmap_insert(ctx, tmp_key, list->values[i]);
            eval(dest, t->children[6], ctx);
        }
        return 0;
    }

    if (strstr(t->tag, "content|text")) {
        strcat(dest, t->contents);
        return 0;
    }

    for (int i=0; i < t->children_num; i++) {
        eval(dest, t->children[i], ctx);
    }
    
    return 0;
}

mpc_parser_t *parser_init() {
    mpc_parser_t *Symbol = mpc_new("symbol");
    mpc_parser_t *Symbols = mpc_new("symbols");
    mpc_parser_t *Text = mpc_new("text");
    mpc_parser_t *Var_Open = mpc_new("var_open");
    mpc_parser_t *Var_Close = mpc_new("var_close");
    mpc_parser_t *Var = mpc_new("var");
    mpc_parser_t *Block_Open = mpc_new("block_open");
    mpc_parser_t *Block_Close = mpc_new("block_close");
    mpc_parser_t *For = mpc_new("for");
    mpc_parser_t *Body = mpc_new("body");
    mpc_parser_t *Content = mpc_new("content");
    mpc_parser_t *Template = mpc_new("template");
    mpca_lang(MPCA_LANG_WHITESPACE_SENSITIVE,
    " symbol    : /[a-zA-Z_]+/ ;"
    " symbols   : <symbol>\".\"?<symbol>? ;"
    " var_open  : /\{{2} ?/ ;"
    " var_close : / ?}{2}/ ;"
    " var       : <var_open> <symbols> <var_close> ;"
    " block_open: /\{\% ?/ ;"
    " block_close: / ?\%}/ ;"
    " for       : <block_open> \"for \" <symbol> \" in \" <symbols> <block_close> <body> <block_open> \"endfor\" <block_close> ;"
    " text      : /[^{][^{%]*/ ;"
    " content   : <var> | <for> | <text>;"
    " body      : <content>* ;"
    " template  : /^/ <body> /$/ ;",
    Symbol, Symbols, Var_Open, Var_Close, Var, Block_Open, Block_Close, For, Text, Content, Body, Template, NULL);
    return Template;
}

char *template(char *tmpl, struct hashmap *ctx) {
    mpc_parser_t *parser = parser_init();
    mpc_result_t r;

    if (!mpc_parse("input", tmpl, parser, &r)) {
        mpc_err_print(r.error);
        mpc_err_delete(r.error);
        return NULL;
    }

    printf("Template: %s\n", tmpl);
    printf("AST: ");
    mpc_ast_print(r.output);
    printf("\n");
        
    // FIXME: Allocate precisely
    char *output = malloc(strlen(tmpl) * 2);
    output[0] = '\0';

    eval(output, r.output, ctx);
    mpc_ast_delete(r.output);

    return output;
}

