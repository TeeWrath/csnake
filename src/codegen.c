#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/codegen.h"

// Forward declarations
void generate_expression(FILE *fp, Expression *expr, int indent);
void generate_statement(FILE *fp, Statement *stmt, int indent);
void generate_block(FILE *fp, Statement *block, int indent);

// Helper function to generate indentation
void print_indent(FILE *fp, int indent) {
    for (int i = 0; i < indent; i++) {
        fprintf(fp, "    ");
    }
}

// Helper function to generate variable type
const char *get_type_name(VariableType type) {
    switch (type) {
        case TYPE_INT: return "int";
        case TYPE_FLOAT: return "float";
        case TYPE_CHAR: return "char";
        case TYPE_STRING: return "str";
        case TYPE_VOID: return "None";
        default: return "unknown";
    }
}

// Generate variable declaration
void generate_variable_declaration(FILE *fp, Variable *var, Expression *initializer, int indent) {
    print_indent(fp, indent);
    fprintf(fp, "%s = ", var->name);
    
    if (initializer != NULL) {
        generate_expression(fp, initializer, 0);
    } else {
        // Default initialization based on type
        if (var->is_array) {
            fprintf(fp, "[");
            if (var->type == TYPE_INT) {
                fprintf(fp, "0");
            } else if (var->type == TYPE_FLOAT) {
                fprintf(fp, "0.0");
            } else if (var->type == TYPE_CHAR) {
                fprintf(fp, "''");
            } else {
                fprintf(fp, "None");
            }
            fprintf(fp, "] * %d", var->array_size);
        } else {
            if (var->type == TYPE_INT) {
                fprintf(fp, "0");
            } else if (var->type == TYPE_FLOAT) {
                fprintf(fp, "0.0");
            } else if (var->type == TYPE_CHAR) {
                fprintf(fp, "''");
            } else {
                fprintf(fp, "None");
            }
        }
    }
    fprintf(fp, "\n");
}

// Generate a literal expression
void generate_literal(FILE *fp, Expression *expr) {
    switch (expr->literal.lit_type) {
        case TYPE_INT:
            fprintf(fp, "%d", expr->literal.int_val);
            break;
            
        case TYPE_FLOAT:
            fprintf(fp, "%f", expr->literal.float_val);
            break;
            
        case TYPE_CHAR:
            fprintf(fp, "'%c'", expr->literal.char_val);
            break;
            
        case TYPE_STRING:
            fprintf(fp, "\"%s\"", expr->literal.string_val);
            break;
            
        default:
            fprintf(fp, "None");
            break;
    }
}

// Generate a variable reference
void generate_variable(FILE *fp, Expression *expr) {
    fprintf(fp, "%s", expr->var_name);
}

// Generate an array access
void generate_array_access(FILE *fp, Expression *expr) {
    fprintf(fp, "%s[", expr->array_access.array_name);
    generate_expression(fp, expr->array_access.index, 0);
    fprintf(fp, "]");
}

// Generate a binary operation
void generate_binary_op(FILE *fp, Expression *expr) {
    // Handle special case for assignment
    if (expr->binary.op == OP_ASSIGN) {
        generate_expression(fp, expr->binary.left, 0);
        fprintf(fp, " = ");
        generate_expression(fp, expr->binary.right, 0);
        return;
    }
    
    // Generate left operand
    fprintf(fp, "(");
    generate_expression(fp, expr->binary.left, 0);
    
    // Generate operator
    switch (expr->binary.op) {
        case OP_ADD: fprintf(fp, " + "); break;
        case OP_SUB: fprintf(fp, " - "); break;
        case OP_MUL: fprintf(fp, " * "); break;
        case OP_DIV: fprintf(fp, " / "); break;
        case OP_MOD: fprintf(fp, " %% "); break;
        case OP_EQ: fprintf(fp, " == "); break;
        case OP_NEQ: fprintf(fp, " != "); break;
        case OP_LT: fprintf(fp, " < "); break;
        case OP_GT: fprintf(fp, " > "); break;
        case OP_LTE: fprintf(fp, " <= "); break;
        case OP_GTE: fprintf(fp, " >= "); break;
        case OP_AND: fprintf(fp, " and "); break;
        case OP_OR: fprintf(fp, " or "); break;
        default: fprintf(fp, " ? "); break;
    }
    
    // Generate right operand
    generate_expression(fp, expr->binary.right, 0);
    fprintf(fp, ")");
}

// Generate a unary operation
void generate_unary_op(FILE *fp, Expression *expr) {
    switch (expr->unary.op) {
        case OP_NEGATE:
            fprintf(fp, "(-");
            generate_expression(fp, expr->unary.expr, 0);
            fprintf(fp, ")");
            break;
            
        case OP_NOT:
            fprintf(fp, "(not ");
            generate_expression(fp, expr->unary.expr, 0);
            fprintf(fp, ")");
            break;
            
        case OP_PRE_INC:
            // C's ++x becomes x += 1 in Python
            fprintf(fp, "(");
            generate_expression(fp, expr->unary.expr, 0);
            fprintf(fp, " += 1)");
            break;
            
        case OP_PRE_DEC:
            // C's --x becomes x -= 1 in Python
            fprintf(fp, "(");
            generate_expression(fp, expr->unary.expr, 0);
            fprintf(fp, " -= 1)");
            break;
            
        case OP_POST_INC:
            // This is tricky in Python, as there's no direct equivalent
            // For simplicity, we'll just do the same as pre-increment
            fprintf(fp, "(");
            generate_expression(fp, expr->unary.expr, 0);
            fprintf(fp, " += 1)");
            break;
            
        case OP_POST_DEC:
            // For simplicity, we'll just do the same as pre-decrement
            fprintf(fp, "(");
            generate_expression(fp, expr->unary.expr, 0);
            fprintf(fp, " -= 1)");
            break;
            
        default:
            fprintf(fp, "?");
            break;
    }
}

// Generate a function call
void generate_function_call(FILE *fp, Expression *expr) {
    fprintf(fp, "%s(", expr->call.func_name);
    
    for (int i = 0; i < expr->call.arg_count; i++) {
        if (i > 0) {
            fprintf(fp, ", ");
        }
        generate_expression(fp, expr->call.args[i], 0);
    }
    
    fprintf(fp, ")");
}

// Generate an expression
void generate_expression(FILE *fp, Expression *expr, int indent) {
    if (expr == NULL) return;
    
    switch (expr->type) {
        case EXPR_LITERAL:
            generate_literal(fp, expr);
            break;
            
        case EXPR_VARIABLE:
            generate_variable(fp, expr);
            break;
            
        case EXPR_BINARY:
            generate_binary_op(fp, expr);
            break;
            
        case EXPR_UNARY:
            generate_unary_op(fp, expr);
            break;
            
        case EXPR_CALL:
            generate_function_call(fp, expr);
            break;
            
        case EXPR_ARRAY_ACCESS:
            generate_array_access(fp, expr);
            break;
            
        default:
            fprintf(fp, "# Unknown expression type\n");
            break;
    }
}

// Generate a block statement
void generate_block(FILE *fp, Statement *block, int indent) {
    for (int i = 0; i < block->block.stmt_count; i++) {
        generate_statement(fp, block->block.statements[i], indent);
    }
}

// Generate an if statement
void generate_if_statement(FILE *fp, Statement *stmt, int indent) {
    print_indent(fp, indent);
    fprintf(fp, "if ");
    generate_expression(fp, stmt->if_stmt.condition, 0);
    fprintf(fp, ":\n");
    
    generate_statement(fp, stmt->if_stmt.then_branch, indent + 1);
    
    if (stmt->if_stmt.else_branch != NULL) {
        print_indent(fp, indent);
        fprintf(fp, "else:\n");
        generate_statement(fp, stmt->if_stmt.else_branch, indent + 1);
    }
}

// Generate a while statement
void generate_while_statement(FILE *fp, Statement *stmt, int indent) {
    print_indent(fp, indent);
    fprintf(fp, "while ");
    generate_expression(fp, stmt->while_stmt.condition, 0);
    fprintf(fp, ":\n");
    
    generate_statement(fp, stmt->while_stmt.body, indent + 1);
}

// Generate a for statement
void generate_for_statement(FILE *fp, Statement *stmt, int indent) {
    // First, generate the initializer
    if (stmt->for_stmt.initializer != NULL) {
        generate_statement(fp, stmt->for_stmt.initializer, indent);
    }
    
    // Generate the while loop
    print_indent(fp, indent);
    fprintf(fp, "while ");
    if (stmt->for_stmt.condition != NULL) {
        generate_expression(fp, stmt->for_stmt.condition, 0);
    } else {
        // No condition means an infinite loop in C's for
        fprintf(fp, "True");
    }
    fprintf(fp, ":\n");
    
    // Generate the loop body
    generate_statement(fp, stmt->for_stmt.body, indent + 1);
    
    // Generate the increment at the end of the loop body
    if (stmt->for_stmt.increment != NULL) {
        print_indent(fp, indent + 1);
        generate_expression(fp, stmt->for_stmt.increment, 0);
        fprintf(fp, "\n");
    }
}

// Generate a return statement
void generate_return_statement(FILE *fp, Statement *stmt, int indent) {
    print_indent(fp, indent);
    fprintf(fp, "return ");
    if (stmt->return_value != NULL) {
        generate_expression(fp, stmt->return_value, 0);
    }
    fprintf(fp, "\n");
}

// Generate a break statement
void generate_break_statement(FILE *fp, Statement *stmt, int indent) {
    print_indent(fp, indent);
    fprintf(fp, "break\n");
}

// Generate a continue statement
void generate_continue_statement(FILE *fp, Statement *stmt, int indent) {
    print_indent(fp, indent);
    fprintf(fp, "continue\n");
}

// Generate a print statement
void generate_print_statement(FILE *fp, Statement *stmt, int indent) {
    print_indent(fp, indent);
    
    // Simple case: just printing one variable
    if (strcmp(stmt->print.format, "%d") == 0 || 
        strcmp(stmt->print.format, "%f") == 0 || 
        strcmp(stmt->print.format, "%c") == 0 || 
        strcmp(stmt->print.format, "%s") == 0) {
        
        if (stmt->print.arg_count == 1) {
            fprintf(fp, "print(");
            generate_expression(fp, stmt->print.args[0], 0);
            fprintf(fp, ")\n");
            return;
        }
    }
    
    // More complex case: formatted string
    fprintf(fp, "print(f\"");
    
    // Convert C format specifiers to Python f-string
    char *format = stmt->print.format;
    int arg_index = 0;
    
    for (int i = 0; format[i] != '\0'; i++) {
        if (format[i] == '%' && format[i+1] != '\0') {
            // Handle format specifier
            switch (format[++i]) {
                case 'd':
                    fprintf(fp, "{");
                    if (arg_index < stmt->print.arg_count) {
                        generate_expression(fp, stmt->print.args[arg_index++], 0);
                    }
                    fprintf(fp, "}");
                    break;
                    
                case 'f':
                    fprintf(fp, "{");
                    if (arg_index < stmt->print.arg_count) {
                        generate_expression(fp, stmt->print.args[arg_index++], 0);
                    }
                    fprintf(fp, "}");
                    break;
                    
                case 'c':
                    fprintf(fp, "{");
                    if (arg_index < stmt->print.arg_count) {
                        generate_expression(fp, stmt->print.args[arg_index++], 0);
                    }
                    fprintf(fp, "}");
                    break;
                    
                case 's':
                    fprintf(fp, "{");
                    if (arg_index < stmt->print.arg_count) {
                        generate_expression(fp, stmt->print.args[arg_index++], 0);
                    }
                    fprintf(fp, "}");
                    break;
                    
                case '%':
                    // Literal '%'
                    fprintf(fp, "%%");
                    break;
                    
                default:
                    // Unknown format specifier, just output it as is
                    fprintf(fp, "%%%c", format[i]);
                    break;
            }
        } else {
            // Regular character
            fputc(format[i], fp);
        }
    }
    
    fprintf(fp, "\")\n");
}

// Generate a statement
void generate_statement(FILE *fp, Statement *stmt, int indent) {
    if (stmt == NULL) return;
    
    switch (stmt->type) {
        case STMT_EXPR:
            print_indent(fp, indent);
            generate_expression(fp, stmt->expr, 0);
            fprintf(fp, "\n");
            break;
            
        case STMT_VAR_DECL:
            generate_variable_declaration(fp, &stmt->var_decl.var, stmt->var_decl.initializer, indent);
            break;
            
        case STMT_BLOCK:
            generate_block(fp, stmt, indent);
            break;
            
        case STMT_IF:
            generate_if_statement(fp, stmt, indent);
            break;
            
        case STMT_WHILE:
            generate_while_statement(fp, stmt, indent);
            break;
            
        case STMT_FOR:
            generate_for_statement(fp, stmt, indent);
            break;
            
        case STMT_RETURN:
            generate_return_statement(fp, stmt, indent);
            break;
            
        case STMT_BREAK:
            generate_break_statement(fp, stmt, indent);
            break;
            
        case STMT_CONTINUE:
            generate_continue_statement(fp, stmt, indent);
            break;
            
        case STMT_PRINT:
            generate_print_statement(fp, stmt, indent);
            break;
            
        default:
            fprintf(fp, "# Unknown statement type\n");
            break;
    }
}

// Generate a function
void generate_function(FILE *fp, Function *func) {
    fprintf(fp, "def %s(", func->name);
    
    // Generate parameters
    for (int i = 0; i < func->param_count; i++) {
        if (i > 0) {
            fprintf(fp, ", ");
        }
        fprintf(fp, "%s", func->params[i].name);
    }
    
    fprintf(fp, "):\n");
    
    // Generate function body
    if (func->body != NULL) {
        generate_block(fp, func->body, 1);
    } else {
        // Empty function body
        fprintf(fp, "    pass\n");
    }
    
    // Add a newline after each function
    fprintf(fp, "\n");
}

// Generate Python code from the parsed C program
void generate_python_code(Program *program, const char *output_file) {
    FILE *fp = fopen(output_file, "w");
    if (!fp) {
        fprintf(stderr, "Error opening output file '%s'\n", output_file);
        return;
    }
    
    // Add a header comment
    fprintf(fp, "# Generated Python code from C source\n");
    fprintf(fp, "# This file was automatically translated by Csnake\n\n");
    
    // Generate global variables
    if (program->global_var_count > 0) {
        fprintf(fp, "# Global variables\n");
        for (int i = 0; i < program->global_var_count; i++) {
            char *default_value = "None";
            if (program->global_vars[i].type == TYPE_INT) {
                default_value = "0";
            } else if (program->global_vars[i].type == TYPE_FLOAT) {
                default_value = "0.0";
            } else if (program->global_vars[i].type == TYPE_CHAR) {
                default_value = "''";
            }
            
            if (program->global_vars[i].is_array) {
                fprintf(fp, "%s = [%s] * %d\n", 
                        program->global_vars[i].name, 
                        default_value,
                        program->global_vars[i].array_size);
            } else {
                fprintf(fp, "%s = %s\n", program->global_vars[i].name, default_value);
            }
        }
        fprintf(fp, "\n");
    }
    
    // Generate functions
    for (int i = 0; i < program->function_count; i++) {
        generate_function(fp, program->functions[i]);
    }
    
    // Add a main function call at the end if main exists
    for (int i = 0; i < program->function_count; i++) {
        if (strcmp(program->functions[i]->name, "main") == 0) {
            fprintf(fp, "# Call main function if this script is run directly\n");
            fprintf(fp, "if __name__ == \"__main__\":\n");
            fprintf(fp, "    main()\n");
            break;
        }
    }
    
    fclose(fp);
    printf("Python code generated in '%s'\n", output_file);
}