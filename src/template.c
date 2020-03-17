#include <stdio.h>
#include <stdlib.h>
#include <err.h>
#include "vendor/mpc.h"
#include "template.h"

enum unja_object_type {
    OBJ_NULL,
    OBJ_INT,
    OBJ_STRING,
};

struct unja_object {
    enum unja_object_type type;
    int integer;
    char *string;
};

struct unja_object null_object = { 
    .type = OBJ_NULL,
};

struct buffer {
    int size;
    int cap;
    char *string;
};

void buffer_reserve(struct buffer *buf, int l) {
    int req_size = buf->size + l;
    if (req_size >= buf->cap) {
        while (req_size >= buf->cap) {
            buf->cap *= 2;
        }
        
        buf->string = realloc(buf->string, buf->cap * sizeof *buf->string);
        if (!buf->string) {
            err(EXIT_FAILURE, "out of memory");
        }
    }
}

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

struct unja_object *make_string_object(char *value, char *value2) {
    struct unja_object *obj = malloc(sizeof *obj);
    obj->type = OBJ_STRING;
    int l = strlen(value) + 1;
    if (value2) {
        l += strlen(value2);
    }

    obj->string = malloc(l);
    strcpy(obj->string, value);
    
    if(value2) {
        strcat(obj->string, value2);
    }
    return obj;
}

struct unja_object *make_int_object(int value) {
    struct unja_object *obj = malloc(sizeof *obj);
    obj->type = OBJ_INT;
    obj->integer = value;
    return obj;
}


void eval_object(struct buffer *buf, struct unja_object *obj) {
    char tmp[64];

    switch (obj->type) {
        case OBJ_NULL: 
            break;
        case OBJ_STRING:
            buffer_reserve(buf, strlen(obj->string));
            strcat(buf->string, obj->string);
            break;
        case OBJ_INT: 
            sprintf(tmp, "%d", obj->integer);
            buffer_reserve(buf, strlen(tmp));
            strcat(buf->string, tmp);
            break;
    }
}

int object_to_int(struct unja_object *obj) {
     switch (obj->type) {
        case OBJ_NULL: return 0; 
        case OBJ_STRING: return atoi(obj->string);
        case OBJ_INT: return obj->integer;
    }

    return 0;
}

void object_free(struct unja_object *obj) {
    switch(obj->type) {
        case OBJ_NULL: return;
        case OBJ_STRING: 
            free(obj->string);
        break;
        case OBJ_INT: break;
    }

    free(obj);
}

int object_is_truthy(struct unja_object *obj) {
    switch (obj->type) {
        case OBJ_NULL: return 0; 
        case OBJ_STRING: return strlen(obj->string) > 0;
        case OBJ_INT: return obj->integer > 0;
    }

    return 0;
}

struct unja_object *eval_expression_value(mpc_ast_t* node, struct hashmap *ctx) {
    if (strstr(node->tag, "symbol|")) {
        /* Return empty string if no context was passed. Should probably signal error here. */
        if (ctx == NULL) {
           return &null_object;
        }

        char *key = node->contents;
        char *value = hashmap_resolve(ctx, key);

        /* TODO: Handle unexisting symbols (returns NULL currently) */
        if (value == NULL) {
            return &null_object;
        }
        return make_string_object(value, NULL);
    } else if(strstr(node->tag, "number|")) {
        return make_int_object(atoi(node->contents));
    } else if(strstr(node->tag, "string|")) {
        return make_string_object(node->children[1]->contents, NULL);
    }

    return &null_object;
}


struct unja_object *eval_expression(mpc_ast_t* expr, struct hashmap *ctx) {
    if (expr->children_num >= 2 && strstr(expr->children[1]->tag, "op")) {
        mpc_ast_t *left_node = expr->children[0];
        mpc_ast_t *op = expr->children[1];
        mpc_ast_t *right_node = expr->children[2];

        struct unja_object *left = eval_expression_value(left_node, ctx);
        struct unja_object *right = eval_expression_value(right_node, ctx);

        /* if operator is + and either left or right node is of type string: concat */
        if (op->contents[0] == '+' && (left->type == OBJ_STRING && right->type == OBJ_STRING)) {           
           struct unja_object *result = make_string_object(left->string, right->string);
           object_free(left);
           object_free(right);
           return result;
        } 

        /* eval int infix expression */
        int result;
        switch (op->contents[0]) {
            case '+': result = object_to_int(left) + object_to_int(right); break;
            case '-': result = object_to_int(left) - object_to_int(right); break;
            case '/': result = object_to_int(left) / object_to_int(right); break;
            case '*': result = object_to_int(left) * object_to_int(right); break;
            case '>': result = object_to_int(left) > object_to_int(right); break;
            case '<': result = object_to_int(left) < object_to_int(right); break;
        }

        object_free(left);
        object_free(right);
        return make_int_object(result);
    }

    mpc_ast_t *left_node = expr;
    return eval_expression_value(left_node, ctx);
}

int eval(struct buffer *buf, mpc_ast_t* t, struct hashmap *ctx) {
    static int trim_whitespace = 0;

    // eval print statement
    if (strstr(t->tag, "content|print")) {
        // maybe eat whitespace going backward
        if (strstr(t->children[1]->contents, "-")) {
            buf->string = trim_trailing_whitespace(buf->string);
        }

        /* set flag for next eval() to trim leading whitespace from text */
        if (strstr(t->children[3]->contents, "-")) {
            trim_whitespace = 1;
        }

        mpc_ast_t *expr = t->children[2];      
        struct unja_object *obj = eval_expression(expr, ctx);  
        eval_object(buf, obj);
        object_free(obj);
        return 0;
    }

    if (strstr(t->tag, "content|statement|for")) {
        char *tmp_key = t->children[2]->contents;
        char *iterator_key = t->children[4]->contents;
        struct vector *list = hashmap_resolve(ctx, iterator_key);
        for (int i=0; i < list->size; i++) {
            hashmap_insert(ctx, tmp_key, list->values[i]);
            eval(buf, t->children[6], ctx);
        }
        return 0;
    }

    if (strstr(t->tag, "content|statement|if")) {
        mpc_ast_t *expr = t->children[2];
        struct unja_object *result = eval_expression(expr, ctx);

        if (object_is_truthy(result)) {
            eval(buf, t->children[4], ctx);
        } else {
            if (t->children_num > 8) {
                eval(buf, t->children[8], ctx);
            }
        }

        object_free(result);
        return 0;
    }

    if (strstr(t->tag, "content|text")) {
        char *str = t->contents;
        if (trim_whitespace) {
            str = trim_leading_whitespace(str);
            trim_whitespace = 0;
        }

        buffer_reserve(buf, strlen(str));
        strcat(buf->string, str);
        return 0;
    }

    for (int i=0; i < t->children_num; i++) {
        eval(buf, t->children[i], ctx);
    }
    
    return 0;
}

mpc_parser_t *parser_init() {
    static mpc_parser_t *template;
    if (template != NULL) {
        return template;
    }

    mpc_parser_t *symbol = mpc_new("symbol");
    mpc_parser_t *number = mpc_new("number");
    mpc_parser_t *string = mpc_new("string");
    mpc_parser_t *op = mpc_new("op");
    mpc_parser_t *text = mpc_new("text");
    mpc_parser_t *print = mpc_new("print");
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
    template = mpc_new("template");
    mpca_lang(MPCA_LANG_DEFAULT,
        " symbol    : /[a-zA-Z][a-zA-Z0-9_.]*/ ;"
        " number    : /[0-9]+/ ;"
        " string    : '\"' /([^\"])*/ '\"' ;"
        " op        : '+' | '-' | '*' | '/' | '>' | '<';"
        " text      : /[^{][^{%#]*/ ;"
        " expression: (<symbol> | <number> | <string>) (<op> (<symbol> | <number> | <string>))* ;"
        " print     : \"{{\" /-? */ <expression> / *-?/ \"}}\" ;"
        " comment : \"{#\" /[^#][^#}]*/ \"#}\" ;"
        " statement_open: \"{%\" /-? */;"
        " statement_close: / *-?/ \"%}\";"
        " for       : <statement_open> \"for \" <symbol> \"in\" <symbol> <statement_close> <body> <statement_open> \"endfor\" <statement_close> ;"
        " block     : <statement_open> \"block \" <symbol> <statement_close> <statement_open> \"endblock\" <statement_close>;"
        " extends   : <statement_open> \"extends \" <string> <statement_close>;"
        " if        : <statement_open> \"if \" <expression> <statement_close> <body> (<statement_open> \"else\" <statement_close> <body>)? <statement_open> \"endif\" <statement_close> ;"
        " statement : <for> | <block> | <extends> | <if> ;"
        " content   : <print> | <statement> | <text> | <comment>;"
        " body      : <content>* ;"
        " template  : /^/ <body> /$/ ;",
        symbol, 
        op,
        number,
        string,
        print,
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
        
    struct buffer buf;
    buf.size = 0;
    buf.cap = strlen(tmpl) + 1;
    buf.string = malloc(buf.size);
    buf.string[0] = '\0';
    eval(&buf, r.output, ctx);

    mpc_ast_delete(r.output);
    return buf.string;
}

