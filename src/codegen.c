#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/parser.h"
#include "../include/codegen.h"

void indent(FILE *fp, int level) {
    for (int i = 0; i < level; i++) {
        fprintf(fp, "    ");
    }
}

void generate_expression(FILE *fp, Expression *expr, int indent_level);
void generate_statement(FILE *fp, Statement *stmt, int indent_level);

const char *get_python_type_name(VariableType type) {
    switch (type) {
        case TYPE_INT: return "int";
        case TYPE_FLOAT: return "float";
        case TYPE_CHAR: return "str";
        case TYPE_STRING: return "str";
        case TYPE_VOID: return "None";
        default: return "Any"; // Fallback for unknown types
    }
}

void generate_type(FILE *fp, VariableType type, char *struct_name) {
    if (struct_name) {
        fprintf(fp, "%s", struct_name);
    } else {
        fprintf(fp, "%s", get_python_type_name(type));
    }
}

void generate_binary_op(FILE *fp, BinaryOpType op) {
    switch (op) {
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
        case OP_ASSIGN: fprintf(fp, " = "); break;
        case OP_BIT_AND: fprintf(fp, " & "); break;
        case OP_BIT_OR: fprintf(fp, " | "); break;
        case OP_BIT_XOR: fprintf(fp, " ^ "); break;
        case OP_SHIFT_LEFT: fprintf(fp, " << "); break;
        case OP_SHIFT_RIGHT: fprintf(fp, " >> "); break;
    }
}

void generate_unary_op(FILE *fp, UnaryOpType op) {
    switch (op) {
        case OP_NEGATE: fprintf(fp, "-"); break;
        case OP_NOT: fprintf(fp, "not "); break;
        case OP_PRE_INC: fprintf(fp, "++"); break;
        case OP_PRE_DEC: fprintf(fp, "--"); break;
        case OP_POST_INC: fprintf(fp, "++"); break;
        case OP_POST_DEC: fprintf(fp, "--"); break;
        case OP_BIT_NOT: fprintf(fp, "~"); break;
    }
}

void generate_expression(FILE *fp, Expression *expr, int indent_level) {
    if (!expr) return;

    switch (expr->type) {
        case EXPR_VARIABLE:
            fprintf(fp, "%s", expr->var_name);
            break;

        case EXPR_LITERAL:
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
            break;

        case EXPR_BINARY:
            if (expr->binary.op == OP_ASSIGN) {
                generate_expression(fp, expr->binary.left, indent_level);
                generate_binary_op(fp, expr->binary.op);
                generate_expression(fp, expr->binary.right, indent_level);
            } else {
                fprintf(fp, "(");
                generate_expression(fp, expr->binary.left, indent_level);
                generate_binary_op(fp, expr->binary.op);
                generate_expression(fp, expr->binary.right, indent_level);
                fprintf(fp, ")");
            }
            break;

        case EXPR_UNARY:
            if (expr->unary.op == OP_POST_INC || expr->unary.op == OP_POST_DEC) {
                generate_expression(fp, expr->unary.expr, indent_level);
                generate_unary_op(fp, expr->unary.op);
            } else {
                generate_unary_op(fp, expr->unary.op);
                generate_expression(fp, expr->unary.expr, indent_level);
            }
            break;

        case EXPR_CALL:
            fprintf(fp, "%s(", expr->call.func_name);
            for (int i = 0; i < expr->call.arg_count; i++) {
                generate_expression(fp, expr->call.args[i], indent_level);
                if (i < expr->call.arg_count - 1) {
                    fprintf(fp, ", ");
                }
            }
            fprintf(fp, ")");
            break;

        case EXPR_ARRAY_ACCESS:
            fprintf(fp, "%s[", expr->array_access.array_name);
            generate_expression(fp, expr->array_access.index, indent_level);
            fprintf(fp, "]");
            break;

        case EXPR_MEMBER_ACCESS:
            generate_expression(fp, expr->member_access.struct_expr, indent_level);
            fprintf(fp, ".%s", expr->member_access.member_name);
            break;

        case EXPR_ASM:
            indent(fp, indent_level);
            fprintf(fp, "# Inline assembly block\n");
            indent(fp, indent_level);
            fprintf(fp, "# Instructions: %s\n", expr->asm_block.instructions ? expr->asm_block.instructions : "");
            if (expr->asm_block.output_count > 0) {
                indent(fp, indent_level);
                fprintf(fp, "# Outputs:\n");
                for (int i = 0; i < expr->asm_block.output_count; i++) {
                    indent(fp, indent_level);
                    fprintf(fp, "#   %s (%s)\n", 
                            expr->asm_block.outputs[i].constraint ? expr->asm_block.outputs[i].constraint : "",
                            expr->asm_block.outputs[i].variable ? expr->asm_block.outputs[i].variable : "");
                }
            }
            if (expr->asm_block.input_count > 0) {
                indent(fp, indent_level);
                fprintf(fp, "# Inputs:\n");
                for (int i = 0; i < expr->asm_block.input_count; i++) {
                    indent(fp, indent_level);
                    fprintf(fp, "#   %s (%s)\n", 
                            expr->asm_block.inputs[i].constraint ? expr->asm_block.inputs[i].constraint : "",
                            expr->asm_block.inputs[i].variable ? expr->asm_block.inputs[i].variable : "");
                }
            }
            if (expr->asm_block.clobber_count > 0) {
                indent(fp, indent_level);
                fprintf(fp, "# Clobbers:");
                for (int i = 0; i < expr->asm_block.clobber_count; i++) {
                    fprintf(fp, " %s", expr->asm_block.clobbers[i]);
                    if (i < expr->asm_block.clobber_count - 1) {
                        fprintf(fp, ",");
                    }
                }
                fprintf(fp, "\n");
            }
            break;
    }
}

void generate_variable_init(FILE *fp, Variable *var, int indent_level) {
    indent(fp, indent_level);
    fprintf(fp, "%s: ", var->name);
    if (var->is_array) {
        fprintf(fp, "List[");
        generate_type(fp, var->type, var->struct_name);
        fprintf(fp, "] = [%s] * %d", 
                var->type == TYPE_INT ? "0" : var->type == TYPE_FLOAT ? "0.0" : "''", 
                var->array_size);
    } else {
        generate_type(fp, var->type, var->struct_name);
        if (!var->is_initialized) {
            fprintf(fp, " = %s", 
                    var->type == TYPE_INT ? "0" : 
                    var->type == TYPE_FLOAT ? "0.0" : 
                    var->type == TYPE_CHAR ? "''" : 
                    var->struct_name ? var->struct_name : "None");
        }
    }
    if (var->is_initialized && var->is_array) {
        fprintf(fp, "  # Array initialized elsewhere");
    }
    fprintf(fp, "\n");
}

void generate_statement(FILE *fp, Statement *stmt, int indent_level) {
    if (!stmt) return;

    switch (stmt->type) {
        case STMT_EXPR:
            indent(fp, indent_level);
            generate_expression(fp, stmt->expr, indent_level);
            fprintf(fp, "\n");
            break;

        case STMT_VAR_DECL:
            generate_variable_init(fp, &stmt->var_decl.var, indent_level);
            if (stmt->var_decl.initializer) {
                indent(fp, indent_level);
                fprintf(fp, "%s = ", stmt->var_decl.var.name);
                generate_expression(fp, stmt->var_decl.initializer, indent_level);
                fprintf(fp, "\n");
            }
            break;

        case STMT_BLOCK:
            for (int i = 0; i < stmt->block.stmt_count; i++) {
                generate_statement(fp, stmt->block.statements[i], indent_level);
            }
            break;

        case STMT_IF:
            indent(fp, indent_level);
            fprintf(fp, "if ");
            generate_expression(fp, stmt->if_stmt.condition, indent_level);
            fprintf(fp, ":\n");
            generate_statement(fp, stmt->if_stmt.then_branch, indent_level + 1);
            if (stmt->if_stmt.else_branch) {
                indent(fp, indent_level);
                fprintf(fp, "else:\n");
                generate_statement(fp, stmt->if_stmt.else_branch, indent_level + 1);
            }
            break;

        case STMT_WHILE:
            indent(fp, indent_level);
            fprintf(fp, "while ");
            generate_expression(fp, stmt->while_stmt.condition, indent_level);
            fprintf(fp, ":\n");
            generate_statement(fp, stmt->while_stmt.body, indent_level + 1);
            break;

        case STMT_FOR:
            if (stmt->for_stmt.initializer) {
                generate_statement(fp, stmt->for_stmt.initializer, indent_level);
            }
            indent(fp, indent_level);
            fprintf(fp, "while ");
            generate_expression(fp, stmt->for_stmt.condition, indent_level);
            fprintf(fp, ":\n");
            generate_statement(fp, stmt->for_stmt.body, indent_level + 1);
            if (stmt->for_stmt.increment) {
                indent(fp, indent_level + 1);
                generate_expression(fp, stmt->for_stmt.increment, indent_level + 1);
                fprintf(fp, "\n");
            }
            break;

        case STMT_RETURN:
            indent(fp, indent_level);
            fprintf(fp, "return");
            if (stmt->return_value) {
                fprintf(fp, " ");
                generate_expression(fp, stmt->return_value, indent_level);
            }
            fprintf(fp, "\n");
            break;

        case STMT_BREAK:
            indent(fp, indent_level);
            fprintf(fp, "break\n");
            break;

        case STMT_CONTINUE:
            indent(fp, indent_level);
            fprintf(fp, "continue\n");
            break;

        case STMT_PRINT:
            indent(fp, indent_level);
            fprintf(fp, "print(f\"%s\"", stmt->print.format);
            for (int i = 0; i < stmt->print.arg_count; i++) {
                fprintf(fp, ", ");
                generate_expression(fp, stmt->print.args[i], indent_level);
            }
            fprintf(fp, ")\n");
            break;
    }
}

void generate_function(FILE *fp, Function *func, int indent_level) {
    indent(fp, indent_level);
    fprintf(fp, "def %s(", func->name);
    for (int i = 0; i < func->param_count; i++) {
        fprintf(fp, "%s: ", func->params[i].name);
        generate_type(fp, func->params[i].type, func->params[i].struct_name);
        if (i < func->param_count - 1) {
            fprintf(fp, ", ");
        }
    }
    fprintf(fp, ") -> ");
    generate_type(fp, func->return_type, NULL);
    fprintf(fp, ":\n");
    generate_statement(fp, func->body, indent_level + 1);
}

void generate_code(Program *prog, const char *output_file) {
    FILE *fp = fopen(output_file, "w");
    if (!fp) {
        fprintf(stderr, "Error: Could not open output file %s\n", output_file);
        return;
    }

    fprintf(fp, "from dataclasses import dataclass\n");
    fprintf(fp, "from typing import List\n\n");

    // Generate structs
    generate_structs(fp, prog->structs, prog->struct_count);

    // Generate global variables
    for (int i = 0; i < prog->global_var_count; i++) {
        generate_variable_init(fp, &prog->global_vars[i], 0);
    }
    if (prog->global_var_count > 0) {
        fprintf(fp, "\n");
    }

    // Generate functions
    for (int i = 0; i < prog->function_count; i++) {
        generate_function(fp, prog->functions[i], 0);
        fprintf(fp, "\n");
    }

    // Generate main execution block
    fprintf(fp, "if __name__ == \"__main__\":\n");
    for (int i = 0; i < prog->function_count; i++) {
        if (strcmp(prog->functions[i]->name, "main") == 0) {
            fprintf(fp, "    main()\n");
            break;
        }
    }

    fclose(fp);
}