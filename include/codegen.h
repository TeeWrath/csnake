#ifndef CODEGEN_H
#define CODEGEN_H

#include "parser.h"

// Function declarations
void generate_code(Program *program, const char *output_file);
const char *get_python_type_name(VariableType type);
void indent(FILE *fp, int indent);
void generate_structs(FILE *fp, Struct *structs, int struct_count);

#endif