#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "../include/lexer.h"

// Helper function to check if a string is a C keyword
int is_keyword(const char *str)
{
    static const char *keywords[] = {
        "int", "float", "char", "void", "if", "else", "while", "for", "do",
        "return", "break", "continue", "printf", "scanf"};
    static const TokenType token_types[] = {
        TOKEN_INT, TOKEN_FLOAT, TOKEN_CHAR, TOKEN_VOID, TOKEN_IF, TOKEN_ELSE, TOKEN_WHILE, TOKEN_FOR, TOKEN_DO,
        TOKEN_RETURN, TOKEN_BREAK, TOKEN_CONTINUE, TOKEN_PRINTF, TOKEN_SCANF};
    static const int keyword_count = sizeof(keywords) / sizeof(keywords[0]);

    for (int i = 0; i < keyword_count; i++)
    {
        if (strcmp(str, keywords[i]) == 0)
        {
            return token_types[i];
        }
    }
    return -1; // Not a keyword
}

// Print token for debugging
void print_token(Token token)
{
    printf("Token type: %d, Value: %s, Line: %d, Col: %d\n",
           token.type, token.value ? token.value : "NULL", token.line, token.column);
}

Token *lexer(const char *input, int *token_count)
{
    // Allocate memory for tokens (dynamically resizable)
    int capacity = 100;
    Token *tokens = malloc(capacity * sizeof(Token));
    *token_count = 0;
    int pos = 0;
    int line = 1;
    int column = 1;

    while (input[pos] != '\0')
    {
        // Skip whitespace
        if (isspace(input[pos]))
        {
            if (input[pos] == '\n')
            {
                line++;
                column = 1;
            }
            else
            {
                column++;
            }
            pos++;
            continue;
        }

        // Skip comments
        if (input[pos] == '/' && input[pos + 1] == '/')
        {
            pos += 2;
            column += 2;
            while (input[pos] != '\0' && input[pos] != '\n')
            {
                pos++;
                column++;
            }
            continue;
        }

        // Skip block comments
        if (input[pos] == '/' && input[pos + 1] == '*')
        {
            pos += 2;
            column += 2;
            while (input[pos] != '\0' && !(input[pos] == '*' && input[pos + 1] == '/'))
            {
                if (input[pos] == '\n')
                {
                    line++;
                    column = 1;
                }
                else
                {
                    column++;
                }
                pos++;
            }
            if (input[pos] != '\0')
            {
                pos += 2; // Skip the closing */
                column += 2;
            }
            continue;
        }

        // Resize token array if needed
        if (*token_count >= capacity - 1)
        {
            capacity *= 2;
            tokens = realloc(tokens, capacity * sizeof(Token));
        }

        // Initialize token line and column
        tokens[*token_count].line = line;
        tokens[*token_count].column = column;

        // Identifiers and keywords
        if (isalpha(input[pos]) || input[pos] == '_')
        {
            char id[256] = {0};
            int i = 0;
            while (isalnum(input[pos]) || input[pos] == '_')
            {
                id[i++] = input[pos++];
                column++;
            }
            id[i] = '\0';

            // Check if it's a keyword
            int keyword_token = is_keyword(id);
            if (keyword_token != -1)
            {
                tokens[*token_count].type = (TokenType)keyword_token;
            }
            else
            {
                tokens[*token_count].type = TOKEN_ID;
            }
            tokens[*token_count].value = strdup(id);
            (*token_count)++;
            continue;
        }

        // Numbers (integers and floats)
        if (isdigit(input[pos]))
        {
            char num[256] = {0};
            int i = 0;
            int is_float = 0;

            while (isdigit(input[pos]) || input[pos] == '.')
            {
                if (input[pos] == '.')
                {
                    is_float = 1;
                }
                num[i++] = input[pos++];
                column++;
            }
            num[i] = '\0';

            tokens[*token_count].type = TOKEN_NUMBER;
            tokens[*token_count].value = strdup(num);
            (*token_count)++;
            continue;
        }

        // Character literals
        if (input[pos] == '\'')
        {
            pos++; // Skip opening quote
            column++;

            char ch[3] = {0}; // Can store a char or an escape sequence
            int i = 0;

            // Handle escape sequences
            if (input[pos] == '\\')
            {
                ch[i++] = input[pos++]; // Add backslash
                column++;
            }

            ch[i++] = input[pos++]; // Add character
            column++;

            if (input[pos] == '\'')
            {
                pos++; // Skip closing quote
                column++;
                tokens[*token_count].type = TOKEN_CHAR_LITERAL;
                tokens[*token_count].value = strdup(ch);
                (*token_count)++;
                continue;
            }
            else
            {
                fprintf(stderr, "Error: Unclosed character literal at line %d, column %d\n", line, column);
                // Skip to next quote or end of line
                while (input[pos] != '\0' && input[pos] != '\'' && input[pos] != '\n')
                {
                    pos++;
                    column++;
                }
                if (input[pos] == '\'')
                {
                    pos++; // Skip closing quote
                    column++;
                }
                continue;
            }
        }

        // String literals
        if (input[pos] == '"')
        {
            pos++; // Skip opening quote
            column++;

            char str[1024] = {0};
            int i = 0;

            while (input[pos] != '\0' && input[pos] != '"')
            {
                if (input[pos] == '\\')
                {
                    // Handle escape sequences
                    pos++;
                    column++;
                    if (input[pos] == 'n')
                    {
                        str[i++] = '\\';
                        str[i++] = 'n';
                    }
                    else if (input[pos] == 't')
                    {
                        str[i++] = '\\';
                        str[i++] = 't';
                    }
                    else if (input[pos] == '"')
                    {
                        str[i++] = '\\';
                        str[i++] = '"';
                    }
                    else
                    {
                        str[i++] = input[pos];
                    }
                }
                else
                {
                    str[i++] = input[pos];
                }
                pos++;
                column++;
            }

            if (input[pos] == '"')
            {
                pos++; // Skip closing quote
                column++;
                tokens[*token_count].type = TOKEN_STRING;
                tokens[*token_count].value = strdup(str);
                (*token_count)++;
                continue;
            }
            else
            {
                fprintf(stderr, "Error: Unclosed string literal at line %d, column %d\n", line, column);
                continue;
            }
        }

        // Operators and punctuation
        switch (input[pos])
        {
        case '+':
            if (input[pos + 1] == '+')
            {
                tokens[*token_count].type = TOKEN_INCR;
                tokens[*token_count].value = strdup("++");
                pos += 2;
                column += 2;
            }
            else
            {
                tokens[*token_count].type = TOKEN_PLUS;
                tokens[*token_count].value = strdup("+");
                pos++;
                column++;
            }
            break;

        case '-':
            if (input[pos + 1] == '-')
            {
                tokens[*token_count].type = TOKEN_DECR;
                tokens[*token_count].value = strdup("--");
                pos += 2;
                column += 2;
            }
            else
            {
                tokens[*token_count].type = TOKEN_MINUS;
                tokens[*token_count].value = strdup("-");
                pos++;
                column++;
            }
            break;

        case '*':
            tokens[*token_count].type = TOKEN_MULTIPLY;
            tokens[*token_count].value = strdup("*");
            pos++;
            column++;
            break;

        case '/':
            tokens[*token_count].type = TOKEN_DIVIDE;
            tokens[*token_count].value = strdup("/");
            pos++;
            column++;
            break;

        case '%':
            tokens[*token_count].type = TOKEN_MOD;
            tokens[*token_count].value = strdup("%");
            pos++;
            column++;
            break;

        case '=':
            if (input[pos + 1] == '=')
            {
                tokens[*token_count].type = TOKEN_EQ;
                tokens[*token_count].value = strdup("==");
                pos += 2;
                column += 2;
            }
            else
            {
                tokens[*token_count].type = TOKEN_EQUALS;
                tokens[*token_count].value = strdup("=");
                pos++;
                column++;
            }
            break;

        case '!':
            if (input[pos + 1] == '=')
            {
                tokens[*token_count].type = TOKEN_NEQ;
                tokens[*token_count].value = strdup("!=");
                pos += 2;
                column += 2;
            }
            else
            {
                tokens[*token_count].type = TOKEN_NOT;
                tokens[*token_count].value = strdup("!");
                pos++;
                column++;
            }
            break;

        case '<':
            if (input[pos + 1] == '=')
            {
                tokens[*token_count].type = TOKEN_LTE;
                tokens[*token_count].value = strdup("<=");
                pos += 2;
                column += 2;
            }
            else if (input[pos + 1] == '<')
            {
                tokens[*token_count].type = TOKEN_SHIFT_LEFT;
                tokens[*token_count].value = strdup("<<");
                pos += 2;
                column += 2;
            }
            else
            {
                tokens[*token_count].type = TOKEN_LT;
                tokens[*token_count].value = strdup("<");
                pos++;
                column++;
            }
            break;

        case '>':
            if (input[pos + 1] == '=')
            {
                tokens[*token_count].type = TOKEN_GTE;
                tokens[*token_count].value = strdup(">=");
                pos += 2;
                column += 2;
            }
            else if (input[pos + 1] == '>')
            {
                tokens[*token_count].type = TOKEN_SHIFT_RIGHT;
                tokens[*token_count].value = strdup(">>");
                pos += 2;
                column += 2;
            }
            else
            {
                tokens[*token_count].type = TOKEN_GT;
                tokens[*token_count].value = strdup(">");
                pos++;
                column++;
            }
            break;

        case '&':
            if (input[pos + 1] == '&')
            {
                tokens[*token_count].type = TOKEN_AND;
                tokens[*token_count].value = strdup("&&");
                pos += 2;
                column += 2;
            }
            else
            {
                tokens[*token_count].type = TOKEN_BIT_AND;
                tokens[*token_count].value = strdup("&");
                pos++;
                column++;
            }
            // else
            // {
            //     fprintf(stderr, "Error: Unexpected character '&' at line %d, column %d\n", line, column);
            //     pos++;
            //     column++;
            //     continue;
            // }
            break;

        case '|':
            if (input[pos + 1] == '|')
            {
                tokens[*token_count].type = TOKEN_OR;
                tokens[*token_count].value = strdup("||");
                pos += 2;
                column += 2;
            }
            else
            {
                tokens[*token_count].type = TOKEN_BIT_OR;
                tokens[*token_count].value = strdup("|");
                pos++;
                column++;
            }
            // else
            // {
            //     fprintf(stderr, "Error: Unexpected character '|' at line %d, column %d\n", line, column);
            //     pos++;
            //     column++;
            //     continue;
            // }
            break;

        case ';':
            tokens[*token_count].type = TOKEN_SEMICOLON;
            tokens[*token_count].value = strdup(";");
            pos++;
            column++;
            break;

        case '(':
            tokens[*token_count].type = TOKEN_LPAREN;
            tokens[*token_count].value = strdup("(");
            pos++;
            column++;
            break;

        case ')':
            tokens[*token_count].type = TOKEN_RPAREN;
            tokens[*token_count].value = strdup(")");
            pos++;
            column++;
            break;

        case '{':
            tokens[*token_count].type = TOKEN_LBRACE;
            tokens[*token_count].value = strdup("{");
            pos++;
            column++;
            break;

        case '}':
            tokens[*token_count].type = TOKEN_RBRACE;
            tokens[*token_count].value = strdup("}");
            pos++;
            column++;
            break;

        case '[':
            tokens[*token_count].type = TOKEN_LBRACKET;
            tokens[*token_count].value = strdup("[");
            pos++;
            column++;
            break;

        case ']':
            tokens[*token_count].type = TOKEN_RBRACKET;
            tokens[*token_count].value = strdup("]");
            pos++;
            column++;
            break;

        case ',':
            tokens[*token_count].type = TOKEN_COMMA;
            tokens[*token_count].value = strdup(",");
            pos++;
            column++;
            break;

        case '.':
            tokens[*token_count].type = TOKEN_DOT;
            tokens[*token_count].value = strdup(".");
            pos++;
            column++;
            break;
        case '^':
            tokens[*token_count].type = TOKEN_BIT_XOR;
            tokens[*token_count].value = strdup("^");
            pos++;
            column++;
            break;

        case '~':
            tokens[*token_count].type = TOKEN_BIT_NOT;
            tokens[*token_count].value = strdup("~");
            pos++;
            column++;
            break;

        default:
            fprintf(stderr, "Error: Unknown character '%c' at line %d, column %d\n",
                    input[pos], line, column);
            pos++;
            column++;
            continue;
        }

        (*token_count)++;
    }

    // Add end token
    tokens[*token_count].type = TOKEN_END;
    tokens[*token_count].value = NULL;
    tokens[*token_count].line = line;
    tokens[*token_count].column = column;
    (*token_count)++;

    return tokens;
}