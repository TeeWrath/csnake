#ifndef LEXER_H
#define LEXER_H

typedef enum {
    // Basic types
    TOKEN_INT, TOKEN_FLOAT, TOKEN_CHAR, TOKEN_VOID,
    
    // Identifiers and literals
    TOKEN_ID, TOKEN_NUMBER, TOKEN_CHAR_LITERAL, TOKEN_STRING,
    
    // Operators
    TOKEN_PLUS, TOKEN_MINUS, TOKEN_MULTIPLY, TOKEN_DIVIDE, TOKEN_MOD,
    TOKEN_EQUALS, TOKEN_EQ, TOKEN_NEQ, TOKEN_LT, TOKEN_GT, TOKEN_LTE, TOKEN_GTE,
    TOKEN_AND, TOKEN_OR, TOKEN_NOT, TOKEN_INCR, TOKEN_DECR,
    
    // Punctuation
    TOKEN_SEMICOLON, TOKEN_LPAREN, TOKEN_RPAREN, TOKEN_LBRACE, TOKEN_RBRACE,
    TOKEN_LBRACKET, TOKEN_RBRACKET, TOKEN_COMMA, TOKEN_DOT,
    
    // Flow control
    TOKEN_IF, TOKEN_ELSE, TOKEN_WHILE, TOKEN_FOR, TOKEN_DO, TOKEN_RETURN, TOKEN_BREAK, TOKEN_CONTINUE,
    
    // Functions
    TOKEN_PRINTF, TOKEN_SCANF,

    // Bitwise operators
    TOKEN_BIT_AND, TOKEN_BIT_OR, TOKEN_BIT_XOR, 
    TOKEN_BIT_NOT, TOKEN_SHIFT_LEFT, TOKEN_SHIFT_RIGHT,
    
    // End token
    TOKEN_END
} TokenType;

typedef struct {
    TokenType type;
    char *value;
    int line;     // Track line number for better error reporting
    int column;   // Track column number for better error reporting
} Token;

Token *lexer(const char *input, int *token_count);
void print_token(Token token);  // For debugging

#endif