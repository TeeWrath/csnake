#ifndef CODEGEN_H
#define CODEGEN_H

#include "parser.h"

// Function declarations
void generate_python_code(Program *program, const char *output_file);
const char *get_python_type_name(VariableType type); // Declare for reuse
void print_indent(FILE *fp, int indent);             // Declare for reuse
void generate_structs(FILE *fp, Struct *structs, int struct_count);

#endif