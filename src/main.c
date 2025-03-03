#include <stdio.h>
#include <stdlib.h>
#include "../include/lexer.h"
#include "../include/parser.h"
#include "../include/codegen.h"

int main() {
    const char *input = "int a = 5; printf(\"%d\", a);";
    int token_count = 0;

    // Lexing
    Token *tokens = lexer(input, &token_count);

    // Parsing
    parse(tokens, token_count);

    // Code Generation
    generate_python_code(vars, var_count, tokens, token_count);

    // Cleanup
    for (int i = 0; i < token_count; i++) {
        free(tokens[i].value);
    }
    free(tokens);
    for (int i = 0; i < var_count; i++) {
        free(vars[i].var_name);
    }
    free(vars);

    return 0;
}