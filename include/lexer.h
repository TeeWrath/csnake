#ifndef LEXER_H
#define LEXER_H

typedef enum {
    TOKEN_INT, TOKEN_ID, TOKEN_NUMBER, TOKEN_PLUS, TOKEN_MINUS,
    TOKEN_MULTIPLY, TOKEN_DIVIDE, TOKEN_EQUALS, TOKEN_SEMICOLON,
    TOKEN_PRINTF, TOKEN_STRING, TOKEN_END,
    TOKEN_LPAREN, TOKEN_RPAREN, TOKEN_COMMA // Added new tokens
} TokenType;

typedef struct {
    TokenType type;
    char *value;
} Token;

Token *lexer(const char *input, int *token_count);

#endif