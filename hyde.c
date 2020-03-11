#include <stdio.h>
#include <stdlib.h>
#include "mpc.h"

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

int main() {
    mpc_parser_t *Symbol  = mpc_new("symbol");
    mpc_parser_t *Text  = mpc_new("text");
    mpc_parser_t *Var_Open  = mpc_new("var_open");
    mpc_parser_t *Var_Close  = mpc_new("var_close");
    mpc_parser_t *Var  = mpc_new("var");
    mpc_parser_t *Block_Open  = mpc_new("block_open");
    mpc_parser_t *Block_Close  = mpc_new("block_close");
    mpc_parser_t *Block  = mpc_new("block");
    mpc_parser_t *For  = mpc_new("for");
    mpc_parser_t *Content  = mpc_new("content");
    mpc_parser_t *Template  = mpc_new("template");
    mpca_lang(MPCA_LANG_DEFAULT,
    " symbol    : /[a-zA-Z._]+/ ;"
    " var_open  : \"{{\" ;"
    " var_close : \"}}\" ;"
    " var       : <var_open> <symbol> <var_close> ;"
    " block_open: \"{%\" ;"
    " block_close: \"%}\" ;"
    " for       : <block_open> \"for\" <symbol> \"in\" <symbol> <block_close> <content>* <block_open> \"endfor\" <block_close> ;"
    " text      : /[^{][^{%]*/ ;"
    " content   : <var> | <for> | <text>;"
    " template  : /^/ <content>* /$/ ;",
    Symbol, Var_Open, Var_Close, Var, Block_Open, Block_Close, For, Text, Content, Template, NULL);

    char *content = read_file("index.tpl");

     mpc_result_t r;

    if (mpc_parse("input", content, Template, &r)) {
        mpc_ast_print(r.output);
        mpc_ast_delete(r.output);
    } else {
        mpc_err_print(r.error);
        mpc_err_delete(r.error);
    }

    mpc_cleanup(11, Symbol, Var_Open, Var_Close, Var, Block_Open, Block_Close, For, Block, Text, Content, Template);
    free(content);
}