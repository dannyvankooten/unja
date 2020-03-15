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

char *eval_expression(mpc_ast_t* node, struct hashmap *ctx) {
    if (strstr(node->tag, "symbol")) {
        /* Return empty string if no context was passed. Should probably signal error here. */
        if (ctx == NULL) {
           return "";
        }

        char *key = node->contents;
        /* TODO: Handle unexisting symbols (returns NULL currently) */
        return hashmap_resolve(ctx, key);
    } else if(strstr(node->tag, "number")) {
        return node->contents;
    } else if(strstr(node->tag, "string")) {
        return node->children[1]->contents;
    }

    return NULL;
}

int eval(char *dest, mpc_ast_t* t, struct hashmap *ctx) {
    static int trim_whitespace = 0;
    char buf[64];

    // eval expression
    if (strstr(t->tag, "content|expression|")) {
        // maybe eat whitespace going backward
        if (strstr(t->children[1]->contents, "-")) {
            dest = trim_trailing_whitespace(dest);
        }

        /* set flag for next eval() to trim leading whitespace from text */
        if (strstr(t->children[3]->contents, "-")) {
            trim_whitespace = 1;
        }
        
        mpc_ast_t *left_node = t->children[2];
        char *lvalue = eval_expression(left_node, ctx);

        mpc_ast_t *op = t->children[4];
        if (op != NULL && strstr(op->tag, "op")) {
            mpc_ast_t *right_node = t->children[6];
            char *rvalue = eval_expression(right_node, ctx);

            /* if operator is + and either left or right node is of type string: concat */
            if (op->contents[0] == '+' && (strstr(left_node->tag, "string") || strstr(right_node->tag, "string"))) {
                sprintf(buf, "%s%s", lvalue, rvalue);
            } else {
                /* eval int infix expression */
                int result;
                switch (op->contents[0]) {
                    case '+': result = atoi(lvalue) + atoi(rvalue); break;
                    case '-': result = atoi(lvalue) - atoi(rvalue); break;
                    case '/': result = atoi(lvalue) / atoi(rvalue); break;
                    case '*': result = atoi(lvalue) * atoi(rvalue); break;
                    case '>': result = atoi(lvalue) > atoi(rvalue); break;
                    case '<': result = atoi(lvalue) < atoi(rvalue); break;
                }
                sprintf(buf, "%d", result);
            }
            strcat(dest, buf);
            return 0;
        }

        strcat(dest, lvalue);    
        return 0;
    }

    if (strstr(t->tag, "content|statement|for")) {
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
    mpc_parser_t *number = mpc_new("number");
    mpc_parser_t *string = mpc_new("string");
    mpc_parser_t *op = mpc_new("op");
    mpc_parser_t *text = mpc_new("text");
    mpc_parser_t *expression = mpc_new("expression");
    mpc_parser_t *comment = mpc_new("comment");
    mpc_parser_t *statement = mpc_new("statement");
    mpc_parser_t *statement_open = mpc_new("statement_open");
    mpc_parser_t *statement_close = mpc_new("statement_close");
    mpc_parser_t *statement_for = mpc_new("for");
    mpc_parser_t *statement_if = mpc_new("if");
    mpc_parser_t *statement_block = mpc_new("block");
    mpc_parser_t *statement_extends = mpc_new("extends");
    mpc_parser_t *body = mpc_new("body");
    mpc_parser_t *content = mpc_new("content");
    mpc_parser_t *template = mpc_new("template");
    mpca_lang(MPCA_LANG_WHITESPACE_SENSITIVE,
        " symbol    : /[a-zA-Z_.]+/ ;"
        " number    : /[0-9]+/ ;"
        " string    : '\"' /([^\"])*/ '\"' ;"
        " op        : '+' | '-' | '*' | '/' | '>' | '<';"
        " text      : /[^{][^{%#]*/ ;"
        " expression : \"{{\" /-? */ (<symbol> | <number> | <string> ) (/ */ <op> / */ (<symbol> | <number> | <string> ))? / *-?/ \"}}\" ;"
        " comment : \"{#\" /[^#][^#}]*/ \"#}\" ;"
        " statement_open: \"{%\" /-? */;"
        " statement_close: / *-?/ \"%}\";"
        " for       : <statement_open> \"for \" <symbol> \" in \" <symbol> <statement_close> <body> <statement_open> \"endfor\" <statement_close> ;"
        " block     : <statement_open> \"block \" <statement_close> <statement_open> \"endblock\" <statement_close>;"
        " extends   : <statement_open> \"extends \" <statement_close>;"
        /* TODO: Extend parser to include expression */
        " if        : <statement_open> \"if \" <statement_close> <body> <statement_open> \"endif\" <statement_close> ;"
        " statement : <for> | <block> | <extends> | <if> ;"
        " content   : <expression> | <statement> | <text> | <comment>;"
        " body      : <content>* ;"
        " template  : /^/ <body> /$/ ;",
        symbol, 
        op,
        number,
        string,
        expression, 
        text, 
        comment,
        statement_open, 
        statement_close, 
        statement,
        statement_if,
        statement_for, 
        statement_block,
        statement_extends,
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

