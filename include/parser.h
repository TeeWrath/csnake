#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"

typedef struct {
    char *var_name;
    int value;
} Variable;

// Declare vars and var_count as extern so they can be shared across files
extern Variable *vars;
extern int var_count;

void parse(Token *tokens, int token_count);

#endif