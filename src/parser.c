
#include <stdlib.h>
#include <string.h>

struct text {
    char *str;
};

struct symbol {
    char *value;
};

struct expression {
    struct symbol symbol;
};

struct statement {

};

/* discard during parsing phase */
struct comment {};


enum node_type {
    NODE_EXPR,
    NODE_COMMENT,
    NODE_TEXT,
};

struct node {
    enum node_type type;
    union content {
        struct expression expr;
        struct comment comment;
        struct text text;
    } content;
};

struct ast {
    int size;
    int cap;
    struct node **nodes;
};

enum token_type {
    TOK_EOF,
    TOK_EXPR_OPEN,
    TOK_EXPR_CLOSE,
    TOK_COMMENT_OPEN,
    TOK_COMMENT_CLOSE,
    TOK_STMT_OPEN,
    TOK_STMT_CLOSE,
    TOK_MINUS,
    TOK_SYMBOL,
    TOK_TEXT,
};

struct token {
    enum token_type type;
    char *literal;
};

struct token gettoken(char *str) {
    struct token t;
    switch (str[0]) {
        case '{':

        break;

        case '}':

        break;

        case '\0': 
            t.type = TOK_EOF;
        break;
    }

    return t;
};

struct ast* parse(char *str) {
    struct ast* ast = malloc(sizeof *ast);
    ast->size = 0;
    ast->cap = 64;
    ast->nodes = malloc(ast->cap * sizeof *ast->nodes);

    for (struct token t = gettoken(str); t.type != TOK_EOF; t= gettoken(str)) {

    }

    return ast;
}