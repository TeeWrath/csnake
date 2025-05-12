#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/codegen.h"

// Forward declarations
void generate_expression(FILE *fp, Expression *expr, int indent);
void generate_statement(FILE *fp, Statement *stmt, int indent);
void generate_block(FILE *fp, Statement *block, int indent);

// Helper function to generate indentation
void print_indent(FILE *fp, int indent)
{
    for (int i = 0; i < indent; i++)
    {
        fprintf(fp, "    ");
    }
}

// Helper function to get Python type name for type hints
const char *get_python_type_name(VariableType type) {
    switch (type) {
        case TYPE_INT: return "int";
        case TYPE_FLOAT: return "float";
        case TYPE_CHAR: return "str";
        case TYPE_STRING: return "str";
        case TYPE_VOID: return "None";
        default: return "Any";
    }
}

// Helper function to generate variable type (for initialization)
const char *get_type_name(VariableType type)
{
    switch (type)
    {
    case TYPE_INT:
        return "int";
    case TYPE_FLOAT:
        return "float";
    case TYPE_CHAR:
        return "char";
    case TYPE_STRING:
        return "str";
    case TYPE_VOID:
        return "None";
    default:
        return "unknown";
    }
}

// Generate variable declaration
void generate_variable_declaration(FILE *fp, Variable *var, Expression *initializer, int indent)
{
    print_indent(fp, indent);
    if (var->struct_name) {
        fprintf(fp, "%s: %s = %s()\n", var->name, var->struct_name, var->struct_name);
    } else {
        fprintf(fp, "%s: %s = ", var->name, get_python_type_name(var->type));
        if (initializer != NULL)
        {
            generate_expression(fp, initializer, indent);
        }
        else
        {
            if (var->is_array)
            {
                fprintf(fp, "[");
                if (var->type == TYPE_INT)
                {
                    fprintf(fp, "0");
                }
                else if (var->type == TYPE_FLOAT)
                {
                    fprintf(fp, "0.0");
                }
                else if (var->type == TYPE_CHAR)
                {
                    fprintf(fp, "''");
                }
                else
                {
                    fprintf(fp, "None");
                }
                fprintf(fp, "] * %d", var->array_size);
            }
            else
            {
                if (var->type == TYPE_INT)
                {
                    fprintf(fp, "0");
                }
                else if (var->type == TYPE_FLOAT)
                {
                    fprintf(fp, "0.0");
                }
                else if (var->type == TYPE_CHAR)
                {
                    fprintf(fp, "''");
                }
                else
                {
                    fprintf(fp, "None");
                }
            }
        }
        fprintf(fp, "\n");
    }
}

// Generate a literal expression
void generate_literal(FILE *fp, Expression *expr)
{
    switch (expr->literal.lit_type)
    {
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
void generate_variable(FILE *fp, Expression *expr)
{
    fprintf(fp, "%s", expr->var_name);
}

// Generate a struct member access (e.g., p.x)
void generate_member_access(FILE *fp, Expression *expr)
{
    generate_expression(fp, expr->member_access.struct_expr, 0);
    fprintf(fp, ".%s", expr->member_access.member_name);
}

// Generate an array access
void generate_array_access(FILE *fp, Expression *expr)
{
    fprintf(fp, "%s[", expr->array_access.array_name);
    generate_expression(fp, expr->array_access.index, 0);
    fprintf(fp, "]");
}

// Generate a binary operation
void generate_binary_op(FILE *fp, Expression *expr)
{
    if (expr->binary.op == OP_ASSIGN)
    {
        generate_expression(fp, expr->binary.left, 0);
        fprintf(fp, " = ");
        generate_expression(fp, expr->binary.right, 0);
        return;
    }

    fprintf(fp, "(");
    generate_expression(fp, expr->binary.left, 0);

    switch (expr->binary.op)
    {
    case OP_ADD:
        fprintf(fp, " + ");
        break;
    case OP_SUB:
        fprintf(fp, " - ");
        break;
    case OP_MUL:
        fprintf(fp, " * ");
        break;
    case OP_DIV:
        fprintf(fp, " / ");
        break;
    case OP_MOD:
        fprintf(fp, " %% ");
        break;
    case OP_EQ:
        fprintf(fp, " == ");
        break;
    case OP_NEQ:
        fprintf(fp, " != ");
        break;
    case OP_LT:
        fprintf(fp, " < ");
        break;
    case OP_GT:
        fprintf(fp, " > ");
        break;
    case OP_LTE:
        fprintf(fp, " <= ");
        break;
    case OP_GTE:
        fprintf(fp, " >= ");
        break;
    case OP_AND:
        fprintf(fp, " and ");
        break;
    case OP_OR:
        fprintf(fp, " or ");
        break;
    case OP_BIT_AND:
        fprintf(fp, " & ");
        break;
    case OP_BIT_OR:
        fprintf(fp, " | ");
        break;
    case OP_BIT_XOR:
        fprintf(fp, " ^ ");
        break;
    case OP_SHIFT_LEFT:
        fprintf(fp, " << ");
        break;
    case OP_SHIFT_RIGHT:
        fprintf(fp, " >> ");
        break;
    default:
        fprintf(fp, " ? ");
        break;
    }

    generate_expression(fp, expr->binary.right, 0);
    fprintf(fp, ")");
}

// Generate a unary operation
void generate_unary_op(FILE *fp, Expression *expr)
{
    switch (expr->unary.op)
    {
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
        fprintf(fp, "(");
        generate_expression(fp, expr->unary.expr, 0);
        fprintf(fp, " += 1)");
        break;
    case OP_BIT_NOT:
        fprintf(fp, "(~");
        generate_expression(fp, expr->unary.expr, 0);
        fprintf(fp, ")");
        break;

    case OP_PRE_DEC:
        fprintf(fp, "(");
        generate_expression(fp, expr->unary.expr, 0);
        fprintf(fp, " -= 1)");
        break;

    case OP_POST_INC:
        fprintf(fp, "(");
        generate_expression(fp, expr->unary.expr, 0);
        fprintf(fp, " += 1)");
        break;

    case OP_POST_DEC:
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
void generate_function_call(FILE *fp, Expression *expr)
{
    fprintf(fp, "%s(", expr->call.func_name);

    for (int i = 0; i < expr->call.arg_count; i++)
    {
        if (i > 0)
        {
            fprintf(fp, ", ");
        }
        generate_expression(fp, expr->call.args[i], 0);
    }

    fprintf(fp, ")");
}

// Generate an expression
void generate_expression(FILE *fp, Expression *expr, int indent)
{
    (void)indent;

    if (expr == NULL)
        return;

    switch (expr->type)
    {
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

    case EXPR_MEMBER_ACCESS:
        generate_member_access(fp, expr);
        break;

    default:
        fprintf(fp, "# Unknown expression type\n");
        break;
    }
}

// Generate a block statement
void generate_block(FILE *fp, Statement *block, int indent)
{
    if (block == NULL) return;
    for (int i = 0; i < block->block.stmt_count; i++)
    {
        generate_statement(fp, block->block.statements[i], indent);
    }
}

// Generate an if statement
void generate_if_statement(FILE *fp, Statement *stmt, int indent)
{
    print_indent(fp, indent);
    fprintf(fp, "if ");
    generate_expression(fp, stmt->if_stmt.condition, 0);
    fprintf(fp, ":\n");

    generate_statement(fp, stmt->if_stmt.then_branch, indent + 1);

    if (stmt->if_stmt.else_branch != NULL)
    {
        print_indent(fp, indent);
        fprintf(fp, "else:\n");
        generate_statement(fp, stmt->if_stmt.else_branch, indent + 1);
    }
}

// Generate a while statement
void generate_while_statement(FILE *fp, Statement *stmt, int indent)
{
    print_indent(fp, indent);
    fprintf(fp, "while ");
    generate_expression(fp, stmt->while_stmt.condition, 0);
    fprintf(fp, ":\n");

    generate_statement(fp, stmt->while_stmt.body, indent + 1);
}

// Generate a for statement
void generate_for_statement(FILE *fp, Statement *stmt, int indent)
{
    if (stmt->for_stmt.initializer != NULL)
    {
        generate_statement(fp, stmt->for_stmt.initializer, indent);
    }

    print_indent(fp, indent);
    fprintf(fp, "while ");
    if (stmt->for_stmt.condition != NULL)
    {
        generate_expression(fp, stmt->for_stmt.condition, 0);
    }
    else
    {
        fprintf(fp, "True");
    }
    fprintf(fp, ":\n");

    generate_statement(fp, stmt->for_stmt.body, indent + 1);

    if (stmt->for_stmt.increment != NULL)
    {
        print_indent(fp, indent + 1);
        generate_expression(fp, stmt->for_stmt.increment, 0);
        fprintf(fp, "\n");
    }
}

// Generate a return statement
void generate_return_statement(FILE *fp, Statement *stmt, int indent)
{
    print_indent(fp, indent);
    fprintf(fp, "return ");
    if (stmt->return_value != NULL)
    {
        generate_expression(fp, stmt->return_value, 0);
    }
    fprintf(fp, "\n");
}

// Generate a break statement
void generate_break_statement(FILE *fp, Statement *stmt, int indent)
{
    (void)stmt;
    print_indent(fp, indent);
    fprintf(fp, "break\n");
}

// Generate a continue statement
void generate_continue_statement(FILE *fp, Statement *stmt, int indent)
{
    (void)stmt;
    print_indent(fp, indent);
    fprintf(fp, "continue\n");
}

// Generate a print statement
void generate_print_statement(FILE *fp, Statement *stmt, int indent)
{
    print_indent(fp, indent);

    if (strcmp(stmt->print.format, "%d") == 0 ||
        strcmp(stmt->print.format, "%f") == 0 ||
        strcmp(stmt->print.format, "%c") == 0 ||
        strcmp(stmt->print.format, "%s") == 0)
    {
        if (stmt->print.arg_count == 1)
        {
            fprintf(fp, "print(");
            generate_expression(fp, stmt->print.args[0], 0);
            fprintf(fp, ")\n");
            return;
        }
    }

    fprintf(fp, "print(f\"");

    char *format = stmt->print.format;
    int arg_index = 0;

    for (int i = 0; format[i] != '\0'; i++)
    {
        if (format[i] == '%' && format[i + 1] != '\0')
        {
            switch (format[++i])
            {
            case 'd':
                fprintf(fp, "{");
                if (arg_index < stmt->print.arg_count)
                {
                    generate_expression(fp, stmt->print.args[arg_index++], 0);
                }
                fprintf(fp, "}");
                break;

            case 'f':
                fprintf(fp, "{");
                if (arg_index < stmt->print.arg_count)
                {
                    generate_expression(fp, stmt->print.args[arg_index++], 0);
                }
                fprintf(fp, "}");
                break;

            case 'c':
                fprintf(fp, "{");
                if (arg_index < stmt->print.arg_count)
                {
                    generate_expression(fp, stmt->print.args[arg_index++], 0);
                }
                fprintf(fp, "}");
                break;

            case 's':
                fprintf(fp, "{");
                if (arg_index < stmt->print.arg_count)
                {
                    generate_expression(fp, stmt->print.args[arg_index++], 0);
                }
                fprintf(fp, "}");
                break;

            case '%':
                fprintf(fp, "%%");
                break;

            default:
                fprintf(fp, "%%%c", format[i]);
                break;
            }
        }
        else
        {
            fputc(format[i], fp);
        }
    }

    fprintf(fp, "\")\n");
}

// Generate a statement
void generate_statement(FILE *fp, Statement *stmt, int indent)
{
    if (stmt == NULL)
        return;

    switch (stmt->type)
    {
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

// Generate Python code from the program
void generate_python_code(Program *program, const char *output_file)
{
    if (program == NULL) {
        fprintf(stderr, "Error: Cannot generate code, program is NULL\n");
        return;
    }

    FILE *fp = fopen(output_file, "w");
    if (!fp)
    {
        fprintf(stderr, "Error: Could not open output file %s\n", output_file);
        return;
    }

    fprintf(fp, "from dataclasses import dataclass, field\n");
    fprintf(fp, "from typing import List\n\n");

    generate_structs(fp, program->structs, program->struct_count);

    fprintf(fp, "# Global variables\n");
    for (int i = 0; i < program->global_var_count; i++)
    {
        generate_variable_declaration(fp, &program->global_vars[i], NULL, 0);
    }
    fprintf(fp, "\n");

    fprintf(fp, "# Functions\n");
    for (int i = 0; i < program->function_count; i++)
    {
        Function *func = program->functions[i];
        if (func == NULL) continue;
        fprintf(fp, "def %s(", func->name);

        for (int j = 0; j < func->param_count; j++)
        {
            if (j > 0)
                fprintf(fp, ", ");
            if (func->params[j].struct_name) {
                fprintf(fp, "%s: %s", func->params[j].name, func->params[j].struct_name);
            } else {
                fprintf(fp, "%s: %s", func->params[j].name, get_python_type_name(func->params[j].type));
            }
        }
        fprintf(fp, ") -> %s:\n", get_python_type_name(func->return_type));

        generate_statement(fp, func->body, 1);
        fprintf(fp, "\n");
    }

    fprintf(fp, "if __name__ == \"__main__\":\n");
    fprintf(fp, "    main()\n");

    fclose(fp);
}