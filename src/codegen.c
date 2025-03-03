#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/codegen.h"

void generate_python_code(Variable *vars, int var_count, Token *tokens, int token_count) {
    FILE *fp = fopen("output.py", "w");
    if (!fp) {
        printf("Error opening output file!\n");
        return;
    }

    int i = 0;
    while (i < token_count && tokens[i].type != TOKEN_END) {
        if (tokens[i].type == TOKEN_INT) {
            i++; // Skip 'int'
            char *var_name = tokens[i].value;
            i++; // Skip variable name
            if (tokens[i].type == TOKEN_EQUALS) {
                i++; // Skip '='
                fprintf(fp, "%s = %s\n", var_name, tokens[i].value);
                i++; // Skip number
            }
            if (tokens[i].type == TOKEN_SEMICOLON) i++; // Skip ';'
        } else if (tokens[i].type == TOKEN_PRINTF) {
            i++; // Skip 'printf'
            if (tokens[i].type == TOKEN_LPAREN) i++; // Skip '('
            if (tokens[i].type == TOKEN_STRING) {
                char *format = tokens[i].value;
                i++; // Skip string
                if (tokens[i].type == TOKEN_COMMA) i++; // Skip ','
                if (strcmp(format, "%d") == 0 && tokens[i].type == TOKEN_ID) {
                    fprintf(fp, "print(%s)\n", tokens[i].value);
                    i++; // Skip variable
                }
                if (tokens[i].type == TOKEN_RPAREN) i++; // Skip ')'
            }
            if (tokens[i].type == TOKEN_SEMICOLON) i++; // Skip ';'
        } else {
            i++;
        }
    }

    fclose(fp);
    printf("Python code generated in 'output.py'\n");
}