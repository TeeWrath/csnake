#ifndef CODEGEN_H
#define CODEGEN_H

#include "parser.h"

void generate_python_code(Variable *vars, int var_count, Token *tokens, int token_count);

#endif