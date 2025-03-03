#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/parser.h"

// Define the global variables here
Variable *vars = NULL;
int var_count = 0;

void parse(Token *tokens, int token_count) {
    vars = malloc(100 * sizeof(Variable));
    int i = 0;

    while (i < token_count && tokens[i].type != TOKEN_END) {
        if (tokens[i].type == TOKEN_INT) {
            i++; // Skip 'int'
            if (tokens[i].type == TOKEN_ID) {
                vars[var_count].var_name = strdup(tokens[i].value);
                i++; // Skip variable name
                if (tokens[i].type == TOKEN_EQUALS) {
                    i++; // Skip '='
                    if (tokens[i].type == TOKEN_NUMBER) {
                        vars[var_count].value = atoi(tokens[i].value);
                        var_count++;
                        i++; // Skip number
                    }
                }
                if (tokens[i].type == TOKEN_SEMICOLON) i++; // Skip ';'
            }
        } else if (tokens[i].type == TOKEN_PRINTF) {
            i++; // Skip 'printf'
            if (tokens[i].type == TOKEN_LPAREN) i++; // Skip '('
            if (tokens[i].type == TOKEN_STRING) i++; // Skip string
            if (tokens[i].type == TOKEN_COMMA) i++; // Skip ','
            if (tokens[i].type == TOKEN_ID) i++; // Skip variable
            if (tokens[i].type == TOKEN_RPAREN) i++; // Skip ')'
            if (tokens[i].type == TOKEN_SEMICOLON) i++; // Skip ';'
        } else {
            i++;
        }
    }
}