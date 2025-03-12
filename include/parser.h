#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"

// Forward declarations
typedef struct Statement Statement;
typedef struct Expression Expression;
typedef struct Function Function;

// Variable types
typedef enum {
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_CHAR,
    TYPE_STRING,
    TYPE_VOID
} VariableType;

// Variable structure
typedef struct {
    char *name;
    VariableType type;
    union {
        int int_val;
        float float_val;
        char char_val;
        char *string_val;
    } value;
    int is_initialized;
    int is_array;
    int array_size;
} Variable;

// Expression types
typedef enum {
    EXPR_VARIABLE,
    EXPR_LITERAL,
    EXPR_BINARY,
    EXPR_UNARY,
    EXPR_CALL,
    EXPR_ARRAY_ACCESS
} ExpressionType;

// Binary operation types
typedef enum {
    OP_ADD, OP_SUB, OP_MUL, OP_DIV, OP_MOD,
    OP_EQ, OP_NEQ, OP_LT, OP_GT, OP_LTE, OP_GTE,
    OP_AND, OP_OR, OP_ASSIGN
} BinaryOpType;

// Unary operation types
typedef enum {
    OP_NEGATE, OP_NOT, OP_PRE_INC, OP_PRE_DEC,
    OP_POST_INC, OP_POST_DEC
} UnaryOpType;

// Expression structure
struct Expression {
    ExpressionType type;
    union {
        // Variable
        char *var_name;
        
        // Literal
        struct {
            VariableType lit_type;
            union {
                int int_val;
                float float_val;
                char char_val;
                char *string_val;
            };
        } literal;
        
        // Binary operation
        struct {
            BinaryOpType op;
            Expression *left;
            Expression *right;
        } binary;
        
        // Unary operation
        struct {
            UnaryOpType op;
            Expression *expr;
        } unary;
        
        // Function call
        struct {
            char *func_name;
            Expression **args;
            int arg_count;
        } call;
        
        // Array access
        struct {
            char *array_name;
            Expression *index;
        } array_access;
    };
};

// Statement types
typedef enum {
    STMT_EXPR,
    STMT_VAR_DECL,
    STMT_BLOCK,
    STMT_IF,
    STMT_WHILE,
    STMT_FOR,
    STMT_RETURN,
    STMT_BREAK,
    STMT_CONTINUE,
    STMT_PRINT
} StatementType;

// Statement structure
struct Statement {
    StatementType type;
    union {
        // Expression statement
        Expression *expr;
        
        // Variable declaration
        struct {
            Variable var;
            Expression *initializer;
        } var_decl;
        
        // Block statement
        struct {
            Statement **statements;
            int stmt_count;
        } block;
        
        // If statement
        struct {
            Expression *condition;
            Statement *then_branch;
            Statement *else_branch;
        } if_stmt;
        
        // While statement
        struct {
            Expression *condition;
            Statement *body;
        } while_stmt;
        
        // For statement
        struct {
            Statement *initializer;
            Expression *condition;
            Expression *increment;
            Statement *body;
        } for_stmt;
        
        // Return statement
        Expression *return_value;
        
        // Print statement
        struct {
            char *format;
            Expression **args;
            int arg_count;
        } print;
    };
};

// Function structure
struct Function {
    char *name;
    VariableType return_type;
    Variable *params;
    int param_count;
    Statement *body;
};

// Program structure
typedef struct {
    Function **functions;
    int function_count;
    Variable *global_vars;
    int global_var_count;
} Program;

// Global variables for the current program
extern Program *program;

// Parser functions
Program *parse(Token *tokens, int token_count);
void free_program(Program *program);

#endif