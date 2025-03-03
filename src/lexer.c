#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "../include/lexer.h"

Token *lexer(const char *input, int *token_count) {
    Token *tokens = malloc(100 * sizeof(Token));
    *token_count = 0;
    int pos = 0;

    while (input[pos] != '\0') {
        if (isspace(input[pos])) {
            pos++;
            continue;
        }

        if (strncmp(&input[pos], "int", 3) == 0) {
            tokens[*token_count].type = TOKEN_INT;
            tokens[*token_count].value = strdup("int");
            pos += 3;
        } else if (strncmp(&input[pos], "printf", 6) == 0) {
            tokens[*token_count].type = TOKEN_PRINTF;
            tokens[*token_count].value = strdup("printf");
            pos += 6;
        } else if (isalpha(input[pos])) {
            char *id = malloc(20);
            int i = 0;
            while (isalnum(input[pos])) id[i++] = input[pos++];
            id[i] = '\0';
            tokens[*token_count].type = TOKEN_ID;
            tokens[*token_count].value = id;
        } else if (isdigit(input[pos])) {
            char *num = malloc(20);
            int i = 0;
            while (isdigit(input[pos])) num[i++] = input[pos++];
            num[i] = '\0';
            tokens[*token_count].type = TOKEN_NUMBER;
            tokens[*token_count].value = num;
        } else if (input[pos] == '+') {
            tokens[*token_count].type = TOKEN_PLUS;
            tokens[*token_count].value = strdup("+");
            pos++;
        } else if (input[pos] == '-') {
            tokens[*token_count].type = TOKEN_MINUS;
            tokens[*token_count].value = strdup("-");
            pos++;
        } else if (input[pos] == '*') {
            tokens[*token_count].type = TOKEN_MULTIPLY;
            tokens[*token_count].value = strdup("*");
            pos++;
        } else if (input[pos] == '/') {
            tokens[*token_count].type = TOKEN_DIVIDE;
            tokens[*token_count].value = strdup("/");
            pos++;
        } else if (input[pos] == '=') {
            tokens[*token_count].type = TOKEN_EQUALS;
            tokens[*token_count].value = strdup("=");
            pos++;
        } else if (input[pos] == ';') {
            tokens[*token_count].type = TOKEN_SEMICOLON;
            tokens[*token_count].value = strdup(";");
            pos++;
        } else if (input[pos] == '"') {
            pos++;
            char *str = malloc(100);
            int i = 0;
            while (input[pos] != '"') str[i++] = input[pos++];
            str[i] = '\0';
            pos++;
            tokens[*token_count].type = TOKEN_STRING;
            tokens[*token_count].value = str;
        } else if (input[pos] == '(') { // Handle left parenthesis
            tokens[*token_count].type = TOKEN_LPAREN;
            tokens[*token_count].value = strdup("(");
            pos++;
        } else if (input[pos] == ')') { // Handle right parenthesis
            tokens[*token_count].type = TOKEN_RPAREN;
            tokens[*token_count].value = strdup(")");
            pos++;
        } else if (input[pos] == ',') { // Handle comma
            tokens[*token_count].type = TOKEN_COMMA;
            tokens[*token_count].value = strdup(",");
            pos++;
        } else {
            printf("Unknown character: %c\n", input[pos]);
            pos++;
        }
        (*token_count)++;
    }

    tokens[*token_count].type = TOKEN_END;
    tokens[*token_count].value = NULL;
    (*token_count)++;
    return tokens;
}