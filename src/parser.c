#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "../include/parser.h"

// Global variables
Program *program = NULL;

// Helper functions for memory allocation
Expression *create_expression() {
    Expression *expr = malloc(sizeof(Expression));
    if (!expr) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
    memset(expr, 0, sizeof(Expression));
    return expr;
}

Statement *create_statement() {
    Statement *stmt = malloc(sizeof(Statement));
    if (!stmt) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
    memset(stmt, 0, sizeof(Statement));
    return stmt;
}

Function *create_function() {
    Function *func = malloc(sizeof(Function));
    if (!func) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
    func->name = NULL;
    func->params = NULL;
    func->param_count = 0;
    func->body = NULL;
    return func;
}

Struct *create_struct() {
    Struct *s = malloc(sizeof(Struct));
    if (!s) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
    s->name = NULL;
    s->fields = NULL;
    s->field_count = 0;
    return s;
}

// Parser state
typedef struct {
    Token *tokens;
    int token_count;
    int current;
} Parser;

// Helper function prototypes
VariableType token_to_var_type(TokenType type, Parser *parser);
Expression *parse_expression(Parser *parser);
Statement *parse_statement(Parser *parser);
Statement *parse_block(Parser *parser);
Function *parse_function(Parser *parser);
Struct *parse_struct(Parser *parser);
Expression *parse_bitwise_and(Parser *parser);
Expression *parse_bitwise_xor(Parser *parser);
Expression *parse_bitwise_or(Parser *parser);
Expression *parse_asm(Parser *parser);
void advance(Parser *parser);
Token peek(Parser *parser);
Token previous(Parser *parser);
int match(Parser *parser, TokenType type);
int check(Parser *parser, TokenType type);
void consume(Parser *parser, TokenType type, const char *message);
int is_at_end(Parser *parser);
void synchronize(Parser *parser);

// Convert token type to variable type
VariableType token_to_var_type(TokenType type, Parser *parser) {
    switch (type) {
        case TOKEN_INT: return TYPE_INT;
        case TOKEN_FLOAT: return TYPE_FLOAT;
        case TOKEN_CHAR: return TYPE_CHAR;
        case TOKEN_VOID: return TYPE_VOID;
        default: 
            fprintf(stderr, "Error: Unknown type at line %d, column %d\n", 
                    peek(parser).line, peek(parser).column);
            return TYPE_INT;
    }
}

// Advance to next token
void advance(Parser *parser) {
    if (!is_at_end(parser)) {
        parser->current++;
    }
}

// Get current token without advancing
Token peek(Parser *parser) {
    return parser->tokens[parser->current];
}

// Get previous token
Token previous(Parser *parser) {
    return parser->tokens[parser->current - 1];
}

// Check if current token matches expected type and advance if it does
int match(Parser *parser, TokenType type) {
    if (check(parser, type)) {
        advance(parser);
        return 1;
    }
    return 0;
}

// Check if current token is of expected type
int check(Parser *parser, TokenType type) {
    if (is_at_end(parser)) return 0;
    return peek(parser).type == type;
}

// Consume token of expected type or report error
void consume(Parser *parser, TokenType type, const char *message) {
    if (check(parser, type)) {
        advance(parser);
        return;
    }
    fprintf(stderr, "Parse error at line %d, column %d: %s\n", 
            peek(parser).line, peek(parser).column, message);
    synchronize(parser);
}

// Check if we've reached the end of tokens
int is_at_end(Parser *parser) {
    return peek(parser).type == TOKEN_END;
}

// Synchronize parser after error
void synchronize(Parser *parser) {
    while (!is_at_end(parser)) {
        if (previous(parser).type == TOKEN_SEMICOLON || previous(parser).type == TOKEN_RBRACE) return;
        switch (peek(parser).type) {
            case TOKEN_INT:
            case TOKEN_FLOAT:
            case TOKEN_CHAR:
            case TOKEN_VOID:
            case TOKEN_IF:
            case TOKEN_WHILE:
            case TOKEN_FOR:
            case TOKEN_RETURN:
            case TOKEN_BREAK:
            case TOKEN_CONTINUE:
            case TOKEN_STRUCT:
            case TOKEN_LBRACE:
            case TOKEN_RBRACE:
            case TOKEN_ASM:
                return;
            default:
                break;
        }
        advance(parser);
    }
}

// Parse inline assembly block
Expression *parse_asm(Parser *parser) {
    Expression *expr = create_expression();
    expr->type = EXPR_ASM;
    expr->asm_block.instructions = NULL;
    expr->asm_block.outputs = NULL;
    expr->asm_block.output_count = 0;
    expr->asm_block.inputs = NULL;
    expr->asm_block.input_count = 0;
    expr->asm_block.clobbers = NULL;
    expr->asm_block.clobber_count = 0;

    Token asm_token = previous(parser); // TOKEN_ASM
    char *asm_str = asm_token.value;
    if (!asm_str || strlen(asm_str) == 0) {
        fprintf(stderr, "Parse error at line %d, column %d: Empty asm block\n",
                asm_token.line, asm_token.column);
        consume(parser, TOKEN_SEMICOLON, "Expected ';' after asm block");
        return expr;
    }

    // Parse the assembly block: "instructions" : "outputs" : "inputs" : "clobbers"
    char *instructions = NULL;
    char *output_str = NULL;
    char *input_str = NULL;
    char *clobber_str = NULL;
    char *ptr = asm_str;
    int in_string = 0;
    char *section_start = ptr;

    // Parse instructions until first colon or end
    while (*ptr && (*ptr != ':' || in_string)) {
        if (*ptr == '"') in_string = !in_string;
        ptr++;
    }
    instructions = strndup(section_start, ptr - section_start);
    if (*ptr == ':') ptr++;

    // Parse outputs
    section_start = ptr;
    in_string = 0;
    while (*ptr && (*ptr != ':' || in_string)) {
        if (*ptr == '"') in_string = !in_string;
        ptr++;
    }
    if (ptr > section_start) {
        output_str = strndup(section_start, ptr - section_start);
    }
    if (*ptr == ':') ptr++;

    // Parse inputs
    section_start = ptr;
    in_string = 0;
    while (*ptr && (*ptr != ':' || in_string)) {
        if (*ptr == '"') in_string = !in_string;
        ptr++;
    }
    if (ptr > section_start) {
        input_str = strndup(section_start, ptr - section_start);
    }
    if (*ptr == ':') ptr++;

    // Parse clobbers
    section_start = ptr;
    in_string = 0;
    while (*ptr && (*ptr != ':' || in_string)) {
        if (*ptr == '"') in_string = !in_string;
        ptr++;
    }
    if (ptr > section_start) {
        clobber_str = strndup(section_start, ptr - section_start);
    }

    // Trim instructions
    if (instructions) {
        char *trimmed = instructions;
        while (isspace(*trimmed)) trimmed++;
        char *end = trimmed + strlen(trimmed) - 1;
        while (end > trimmed && isspace(*end)) *end-- = '\0';
        expr->asm_block.instructions = strdup(trimmed);
        free(instructions);
    }

    // Parse outputs
    if (output_str && strlen(output_str) > 0) {
        expr->asm_block.outputs = malloc(10 * sizeof(AsmOperand));
        expr->asm_block.output_count = 0;
        int capacity = 10;
        char *token = strtok(output_str, ",");
        while (token) {
            if (expr->asm_block.output_count >= capacity) {
                capacity *= 2;
                expr->asm_block.outputs = realloc(expr->asm_block.outputs, capacity * sizeof(AsmOperand));
            }
            char *constraint = NULL;
            char *var = NULL;
            // Expected format: "=r" (var)
            char *paren = strchr(token, '(');
            if (paren) {
                *paren = '\0';
                var = paren + 1;
                char *end_paren = strchr(var, ')');
                if (end_paren) *end_paren = '\0';
                constraint = token;
                while (isspace(*constraint)) constraint++;
                char *end = constraint + strlen(constraint) - 1;
                while (end > constraint && isspace(*end)) *end-- = '\0';
                var = strdup(var);
                char *var_end = var + strlen(var) - 1;
                while (var_end > var && isspace(*var_end)) *var_end-- = '\0';
            }
            expr->asm_block.outputs[expr->asm_block.output_count].constraint = constraint ? strdup(constraint) : NULL;
            expr->asm_block.outputs[expr->asm_block.output_count].variable = var ? strdup(var) : NULL;
            expr->asm_block.output_count++;
            token = strtok(NULL, ",");
        }
        free(output_str);
    }

    // Parse inputs
    if (input_str && strlen(input_str) > 0) {
        expr->asm_block.inputs = malloc(10 * sizeof(AsmOperand));
        expr->asm_block.input_count = 0;
        int capacity = 10;
        char *token = strtok(input_str, ",");
        while (token) {
            if (expr->asm_block.input_count >= capacity) {
                capacity *= 2;
                expr->asm_block.inputs = realloc(expr->asm_block.inputs, capacity * sizeof(AsmOperand));
            }
            char *constraint = NULL;
            char *var = NULL;
            char *paren = strchr(token, '(');
            if (paren) {
                *paren = '\0';
                var = paren + 1;
                char *end_paren = strchr(var, ')');
                if (end_paren) *end_paren = '\0';
                constraint = token;
                while (isspace(*constraint)) constraint++;
                char *end = constraint + strlen(constraint) - 1;
                while (end > constraint && isspace(*end)) *end-- = '\0';
                var = strdup(var);
                char *var_end = var + strlen(var) - 1;
                while (var_end > var && isspace(*var_end)) *var_end-- = '\0';
            }
            expr->asm_block.inputs[expr->asm_block.input_count].constraint = constraint ? strdup(constraint) : NULL;
            expr->asm_block.inputs[expr->asm_block.input_count].variable = var ? strdup(var) : NULL;
            expr->asm_block.input_count++;
            token = strtok(NULL, ",");
        }
        free(input_str);
    }

    // Parse clobbers
    if (clobber_str && strlen(clobber_str) > 0) {
        expr->asm_block.clobbers = malloc(10 * sizeof(char*));
        expr->asm_block.clobber_count = 0;
        int capacity = 10;
        char *token = strtok(clobber_str, ",");
        while (token) {
            if (expr->asm_block.clobber_count >= capacity) {
                capacity *= 2;
                expr->asm_block.clobbers = realloc(expr->asm_block.clobbers, capacity * sizeof(char*));
            }
            while (isspace(*token)) token++;
            char *end = token + strlen(token) - 1;
            while (end > token && isspace(*end)) *end-- = '\0';
            expr->asm_block.clobbers[expr->asm_block.clobber_count++] = strdup(token);
            token = strtok(NULL, ",");
        }
        free(clobber_str);
    }

    consume(parser, TOKEN_SEMICOLON, "Expected ';' after asm block");
    return expr;
}

// Parse primary expression (literals, variables, parenthesized expressions, member access, asm)
Expression *parse_primary(Parser *parser) {
    if (match(parser, TOKEN_ASM)) {
        return parse_asm(parser);
    }

    Expression *expr = create_expression();
    
    if (match(parser, TOKEN_NUMBER)) {
        expr->type = EXPR_LITERAL;
        expr->literal.lit_type = TYPE_INT;
        if (strchr(previous(parser).value, '.') != NULL) {
            expr->literal.lit_type = TYPE_FLOAT;
            expr->literal.float_val = atof(previous(parser).value);
        } else {
            expr->literal.int_val = atoi(previous(parser).value);
        }
        return expr;
    }
    
    if (match(parser, TOKEN_CHAR_LITERAL)) {
        expr->type = EXPR_LITERAL;
        expr->literal.lit_type = TYPE_CHAR;
        expr->literal.char_val = previous(parser).value[0];
        return expr;
    }
    
    if (match(parser, TOKEN_STRING)) {
        expr->type = EXPR_LITERAL;
        expr->literal.lit_type = TYPE_STRING;
        expr->literal.string_val = strdup(previous(parser).value);
        return expr;
    }
    
    if (match(parser, TOKEN_ID)) {
        char *name = strdup(previous(parser).value);
        
        // Check if it's a function call
        if (match(parser, TOKEN_LPAREN)) {
            expr->type = EXPR_CALL;
            expr->call.func_name = name;
            expr->call.args = malloc(10 * sizeof(Expression*));
            expr->call.arg_count = 0;
            if (!check(parser, TOKEN_RPAREN)) {
                do {
                    if (expr->call.arg_count >= 10) {
                        expr->call.args = realloc(expr->call.args, 
                                                 (expr->call.arg_count + 10) * sizeof(Expression*));
                    }
                    expr->call.args[expr->call.arg_count++] = parse_expression(parser);
                } while (match(parser, TOKEN_COMMA));
            }
            consume(parser, TOKEN_RPAREN, "Expected ')' after arguments");
            return expr;
        }
        
        // Check if it's an array access
        if (match(parser, TOKEN_LBRACKET)) {
            expr->type = EXPR_ARRAY_ACCESS;
            expr->array_access.array_name = name;
            expr->array_access.index = parse_expression(parser);
            consume(parser, TOKEN_RBRACKET, "Expected ']' after array index");
            return expr;
        }
        
        // It's a regular variable reference
        expr->type = EXPR_VARIABLE;
        expr->var_name = name;
        
        // Check for struct member access (e.g., p.x)
        while (match(parser, TOKEN_DOT)) {
            Expression *member_expr = create_expression();
            member_expr->type = EXPR_MEMBER_ACCESS;
            member_expr->member_access.struct_expr = expr;
            consume(parser, TOKEN_ID, "Expected member name after '.'");
            member_expr->member_access.member_name = strdup(previous(parser).value);
            expr = member_expr;
        }
        
        return expr;
    }
    
    if (match(parser, TOKEN_LPAREN)) {
        Expression *grouping = parse_expression(parser);
        consume(parser, TOKEN_RPAREN, "Expected ')' after expression");
        free(expr);
        return grouping;
    }
    
    fprintf(stderr, "Parse error at line %d, column %d: Expected expression\n", 
            peek(parser).line, peek(parser).column);
    return expr;
}

// Parse unary expressions
Expression *parse_unary(Parser *parser) {
    if (match(parser, TOKEN_MINUS) || match(parser, TOKEN_NOT) || 
        match(parser, TOKEN_INCR) || match(parser, TOKEN_DECR) || 
        match(parser, TOKEN_BIT_NOT)) {
        Expression *expr = create_expression();
        expr->type = EXPR_UNARY;
        if (previous(parser).type == TOKEN_MINUS) {
            expr->unary.op = OP_NEGATE;
        } else if (previous(parser).type == TOKEN_NOT) {
            expr->unary.op = OP_NOT;
        } else if (previous(parser).type == TOKEN_INCR) {
            expr->unary.op = OP_PRE_INC;
        } else if (previous(parser).type == TOKEN_DECR) {
            expr->unary.op = OP_PRE_DEC;
        } else if (previous(parser).type == TOKEN_BIT_NOT) {
            expr->unary.op = OP_BIT_NOT;
        }
        expr->unary.expr = parse_unary(parser);
        return expr;
    }
    
    Expression *expr = parse_primary(parser);
    
    if (match(parser, TOKEN_INCR)) {
        Expression *postfix = create_expression();
        postfix->type = EXPR_UNARY;
        postfix->unary.op = OP_POST_INC;
        postfix->unary.expr = expr;
        return postfix;
    }
    
    if (match(parser, TOKEN_DECR)) {
        Expression *postfix = create_expression();
        postfix->type = EXPR_UNARY;
        postfix->unary.op = OP_POST_DEC;
        postfix->unary.expr = expr;
        return postfix;
    }
    
    return expr;
}

// Parse factor expressions (*, /, %)
Expression *parse_factor(Parser *parser) {
    Expression *expr = parse_unary(parser);
    while (match(parser, TOKEN_MULTIPLY) || match(parser, TOKEN_DIVIDE) || match(parser, TOKEN_MOD)) {
        Expression *binary = create_expression();
        binary->type = EXPR_BINARY;
        if (previous(parser).type == TOKEN_MULTIPLY) {
            binary->binary.op = OP_MUL;
        } else if (previous(parser).type == TOKEN_DIVIDE) {
            binary->binary.op = OP_DIV;
        } else if (previous(parser).type == TOKEN_MOD) {
            binary->binary.op = OP_MOD;
        }
        binary->binary.left = expr;
        binary->binary.right = parse_unary(parser);
        expr = binary;
    }
    return expr;
}

// Parse term expressions (+, -, <<, >>)
Expression *parse_term(Parser *parser) {
    Expression *expr = parse_factor(parser);
    while (match(parser, TOKEN_PLUS) || match(parser, TOKEN_MINUS) ||
           match(parser, TOKEN_SHIFT_LEFT) || match(parser, TOKEN_SHIFT_RIGHT)) {
        Expression *binary = create_expression();
        binary->type = EXPR_BINARY;
        if (previous(parser).type == TOKEN_PLUS) {
            binary->binary.op = OP_ADD;
        } else if (previous(parser).type == TOKEN_MINUS) {
            binary->binary.op = OP_SUB;
        } else if (previous(parser).type == TOKEN_SHIFT_LEFT) {
            binary->binary.op = OP_SHIFT_LEFT;
        } else if (previous(parser).type == TOKEN_SHIFT_RIGHT) {
            binary->binary.op = OP_SHIFT_RIGHT;
        }
        binary->binary.left = expr;
        binary->binary.right = parse_factor(parser);
        expr = binary;
    }
    return expr;
}

// Parse comparison expressions (<, >, <=, >=)
Expression *parse_comparison(Parser *parser) {
    Expression *expr = parse_term(parser);
    while (match(parser, TOKEN_LT) || match(parser, TOKEN_GT) || 
           match(parser, TOKEN_LTE) || match(parser, TOKEN_GTE)) {
        Expression *binary = create_expression();
        binary->type = EXPR_BINARY;
        if (previous(parser).type == TOKEN_LT) {
            binary->binary.op = OP_LT;
        } else if (previous(parser).type == TOKEN_GT) {
            binary->binary.op = OP_GT;
        } else if (previous(parser).type == TOKEN_LTE) {
            binary->binary.op = OP_LTE;
        } else if (previous(parser).type == TOKEN_GTE) {
            binary->binary.op = OP_GTE;
        }
        binary->binary.left = expr;
        binary->binary.right = parse_term(parser);
        expr = binary;
    }
    return expr;
}

// Parse equality expressions (==, !=)
Expression *parse_equality(Parser *parser) {
    Expression *expr = parse_comparison(parser);
    while (match(parser, TOKEN_EQ) || match(parser, TOKEN_NEQ)) {
        Expression *binary = create_expression();
        binary->type = EXPR_BINARY;
        if (previous(parser).type == TOKEN_EQ) {
            binary->binary.op = OP_EQ;
        } else if (previous(parser).type == TOKEN_NEQ) {
            binary->binary.op = OP_NEQ;
        }
        binary->binary.left = expr;
        binary->binary.right = parse_comparison(parser);
        expr = binary;
    }
    return expr;
}

// Parse bitwise AND expressions (&)
Expression *parse_bitwise_and(Parser *parser) {
    Expression *expr = parse_equality(parser);
    while (match(parser, TOKEN_BIT_AND)) {
        Expression *binary = create_expression();
        binary->type = EXPR_BINARY;
        binary->binary.op = OP_BIT_AND;
        binary->binary.left = expr;
        binary->binary.right = parse_equality(parser);
        expr = binary;
    }
    return expr;
}

// Parse bitwise XOR expressions (^)
Expression *parse_bitwise_xor(Parser *parser) {
    Expression *expr = parse_bitwise_and(parser);
    while (match(parser, TOKEN_BIT_XOR)) {
        Expression *binary = create_expression();
        binary->type = EXPR_BINARY;
        binary->binary.op = OP_BIT_XOR;
        binary->binary.left = expr;
        binary->binary.right = parse_bitwise_and(parser);
        expr = binary;
    }
    return expr;
}

// Parse bitwise OR expressions (|)
Expression *parse_bitwise_or(Parser *parser) {
    Expression *expr = parse_bitwise_xor(parser);
    while (match(parser, TOKEN_BIT_OR)) {
        Expression *binary = create_expression();
        binary->type = EXPR_BINARY;
        binary->binary.op = OP_BIT_OR;
        binary->binary.left = expr;
        binary->binary.right = parse_bitwise_xor(parser);
        expr = binary;
    }
    return expr;
}

// Parse logical AND expressions (&&)
Expression *parse_and(Parser *parser) {
    Expression *expr = parse_bitwise_or(parser);
    while (match(parser, TOKEN_AND)) {
        Expression *binary = create_expression();
        binary->type = EXPR_BINARY;
        binary->binary.op = OP_AND;
        binary->binary.left = expr;
        binary->binary.right = parse_bitwise_or(parser);
        expr = binary;
    }
    return expr;
}

// Parse logical OR expressions (||)
Expression *parse_or(Parser *parser) {
    Expression *expr = parse_and(parser);
    while (match(parser, TOKEN_OR)) {
        Expression *binary = create_expression();
        binary->type = EXPR_BINARY;
        binary->binary.op = OP_OR;
        binary->binary.left = expr;
        binary->binary.right = parse_and(parser);
        expr = binary;
    }
    return expr;
}

// Parse assignment expressions
Expression *parse_assignment(Parser *parser) {
    Expression *expr = parse_or(parser);
    if (match(parser, TOKEN_EQUALS)) {
        Expression *binary = create_expression();
        binary->type = EXPR_BINARY;
        binary->binary.op = OP_ASSIGN;
        if (expr->type != EXPR_VARIABLE && expr->type != EXPR_ARRAY_ACCESS && 
            expr->type != EXPR_MEMBER_ACCESS) {
            fprintf(stderr, "Parse error at line %d, column %d: Invalid assignment target\n", 
                    peek(parser).line, peek(parser).column);
        }
        binary->binary.left = expr;
        binary->binary.right = parse_assignment(parser);
        return binary;
    }
    return expr;
}

// Main expression parsing function
Expression *parse_expression(Parser *parser) {
    return parse_assignment(parser);
}

// Parse variable declaration
Statement *parse_var_declaration(Parser *parser, VariableType type, char *struct_name) {
    Statement *stmt = create_statement();
    stmt->type = STMT_VAR_DECL;
    consume(parser, TOKEN_ID, "Expected variable name");
    stmt->var_decl.var.name = strdup(previous(parser).value);
    stmt->var_decl.var.type = type;
    stmt->var_decl.var.is_initialized = 0;
    stmt->var_decl.var.is_array = 0;
    stmt->var_decl.var.struct_name = struct_name ? strdup(struct_name) : NULL;
    
    if (match(parser, TOKEN_LBRACKET)) {
        stmt->var_decl.var.is_array = 1;
        if (match(parser, TOKEN_NUMBER)) {
            stmt->var_decl.var.array_size = atoi(previous(parser).value);
        } else {
            fprintf(stderr, "Parse error at line %d, column %d: Expected array size\n", 
                    peek(parser).line, peek(parser).column);
            stmt->var_decl.var.array_size = 0;
        }
        consume(parser, TOKEN_RBRACKET, "Expected ']' after array size");
    }
    
    if (match(parser, TOKEN_EQUALS)) {
        stmt->var_decl.var.is_initialized = 1;
        stmt->var_decl.initializer = parse_expression(parser);
    } else {
        stmt->var_decl.initializer = NULL;
    }
    
    consume(parser, TOKEN_SEMICOLON, "Expected ';' after variable declaration");
    return stmt;
}

// Parse expression statement
Statement *parse_expression_statement(Parser *parser) {
    Statement *stmt = create_statement();
    stmt->type = STMT_EXPR;
    stmt->expr = parse_expression(parser);
    consume(parser, TOKEN_SEMICOLON, "Expected ';' after expression");
    return stmt;
}

// Parse print statement (printf)
Statement *parse_print_statement(Parser *parser) {
    Statement *stmt = create_statement();
    stmt->type = STMT_PRINT;
    consume(parser, TOKEN_LPAREN, "Expected '(' after 'printf'");
    if (match(parser, TOKEN_STRING)) {
        stmt->print.format = strdup(previous(parser).value);
    } else {
        fprintf(stderr, "Parse error at line %d, column %d: Expected format string\n", 
                peek(parser).line, peek(parser).column);
        stmt->print.format = strdup("");
    }
    
    stmt->print.args = malloc(10 * sizeof(Expression*));
    stmt->print.arg_count = 0;
    while (match(parser, TOKEN_COMMA)) {
        if (stmt->print.arg_count >= 10) {
            stmt->print.args = realloc(stmt->print.args, 
                                      (stmt->print.arg_count + 10) * sizeof(Expression*));
        }
        stmt->print.args[stmt->print.arg_count++] = parse_expression(parser);
    }
    
    consume(parser, TOKEN_RPAREN, "Expected ')' after printf arguments");
    consume(parser, TOKEN_SEMICOLON, "Expected ';' after printf statement");
    return stmt;
}

// Parse block statement
Statement *parse_block(Parser *parser) {
    Statement *stmt = create_statement();
    stmt->type = STMT_BLOCK;
    stmt->block.statements = malloc(10 * sizeof(Statement*));
    stmt->block.stmt_count = 0;
    int capacity = 10;
    
    consume(parser, TOKEN_LBRACE, "Expected '{' at start of block");
    while (!check(parser, TOKEN_RBRACE) && !is_at_end(parser)) {
        if (stmt->block.stmt_count >= capacity) {
            capacity *= 2;
            stmt->block.statements = realloc(stmt->block.statements, capacity * sizeof(Statement*));
        }
        Statement *next_stmt = parse_statement(parser);
        if (next_stmt) {
            stmt->block.statements[stmt->block.stmt_count++] = next_stmt;
        }
    }
    
    consume(parser, TOKEN_RBRACE, "Expected '}' at end of block");
    return stmt;
}

// Parse if statement
Statement *parse_if_statement(Parser *parser) {
    Statement *stmt = create_statement();
    stmt->type = STMT_IF;
    consume(parser, TOKEN_LPAREN, "Expected '(' after 'if'");
    stmt->if_stmt.condition = parse_expression(parser);
    consume(parser, TOKEN_RPAREN, "Expected ')' after if condition");
    stmt->if_stmt.then_branch = parse_statement(parser);
    
    stmt->if_stmt.else_branch = NULL;
    if (match(parser, TOKEN_ELSE)) {
        stmt->if_stmt.else_branch = parse_statement(parser);
    }
    
    return stmt;
}

// Parse while statement
Statement *parse_while_statement(Parser *parser) {
    Statement *stmt = create_statement();
    stmt->type = STMT_WHILE;
    consume(parser, TOKEN_LPAREN, "Expected '(' after 'while'");
    stmt->while_stmt.condition = parse_expression(parser);
    consume(parser, TOKEN_RPAREN, "Expected ')' after while condition");
    stmt->while_stmt.body = parse_statement(parser);
    return stmt;
}

// Parse for statement
Statement *parse_for_statement(Parser *parser) {
    Statement *stmt = create_statement();
    stmt->type = STMT_FOR;
    consume(parser, TOKEN_LPAREN, "Expected '(' after 'for'");
    
    if (match(parser, TOKEN_INT) || match(parser, TOKEN_FLOAT) || match(parser, TOKEN_CHAR)) {
        TokenType type_token = previous(parser).type;
        stmt->for_stmt.initializer = parse_var_declaration(parser, token_to_var_type(type_token, parser), NULL);
    } else if (match(parser, TOKEN_SEMICOLON)) {
        stmt->for_stmt.initializer = NULL;
    } else if (match(parser, TOKEN_STRUCT)) {
        consume(parser, TOKEN_ID, "Expected struct name");
        char *struct_name = strdup(previous(parser).value);
        stmt->for_stmt.initializer = parse_var_declaration(parser, TYPE_VOID, struct_name);
        free(struct_name);
    } else {
        stmt->for_stmt.initializer = parse_expression_statement(parser);
    }
    
    if (!check(parser, TOKEN_SEMICOLON)) {
        stmt->for_stmt.condition = parse_expression(parser);
    } else {
        stmt->for_stmt.condition = create_expression();
        stmt->for_stmt.condition->type = EXPR_LITERAL;
        stmt->for_stmt.condition->literal.lit_type = TYPE_INT;
        stmt->for_stmt.condition->literal.int_val = 1;
    }
    consume(parser, TOKEN_SEMICOLON, "Expected ';' after for condition");
    
    if (!check(parser, TOKEN_RPAREN)) {
        stmt->for_stmt.increment = parse_expression(parser);
    } else {
        stmt->for_stmt.increment = NULL;
    }
    consume(parser, TOKEN_RPAREN, "Expected ')' after for clauses");
    
    stmt->for_stmt.body = parse_statement(parser);
    return stmt;
}

// Parse return statement
Statement *parse_return_statement(Parser *parser) {
    Statement *stmt = create_statement();
    stmt->type = STMT_RETURN;
    if (!check(parser, TOKEN_SEMICOLON)) {
        stmt->return_value = parse_expression(parser);
    } else {
        stmt->return_value = NULL;
    }
    consume(parser, TOKEN_SEMICOLON, "Expected ';' after return value");
    return stmt;
}

// Parse break statement
Statement *parse_break_statement(Parser *parser) {
    Statement *stmt = create_statement();
    stmt->type = STMT_BREAK;
    consume(parser, TOKEN_SEMICOLON, "Expected ';' after 'break'");
    return stmt;
}

// Parse continue statement
Statement *parse_continue_statement(Parser *parser) {
    Statement *stmt = create_statement();
    stmt->type = STMT_CONTINUE;
    consume(parser, TOKEN_SEMICOLON, "Expected ';' after 'continue'");
    return stmt;
}

// Parse struct definition
Struct *parse_struct(Parser *parser) {
    Struct *s = create_struct();
    consume(parser, TOKEN_ID, "Expected struct name");
    s->name = strdup(previous(parser).value);
    consume(parser, TOKEN_LBRACE, "Expected '{' after struct name");
    
    s->fields = malloc(10 * sizeof(Variable));
    s->field_count = 0;
    int capacity = 10;
    
    while (!check(parser, TOKEN_RBRACE) && !is_at_end(parser)) {
        if (s->field_count >= capacity) {
            capacity *= 2;
            s->fields = realloc(s->fields, capacity * sizeof(Variable));
        }
        if (match(parser, TOKEN_INT) || match(parser, TOKEN_FLOAT) || match(parser, TOKEN_CHAR)) {
            s->fields[s->field_count].type = token_to_var_type(previous(parser).type, parser);
        } else {
            fprintf(stderr, "Parse error at line %d, column %d: Expected field type\n", 
                    peek(parser).line, peek(parser).column);
            s->fields[s->field_count].type = TYPE_INT;
        }
        
        consume(parser, TOKEN_ID, "Expected field name");
        s->fields[s->field_count].name = strdup(previous(parser).value);
        s->fields[s->field_count].is_array = 0;
        s->fields[s->field_count].struct_name = NULL;
        
        if (match(parser, TOKEN_LBRACKET)) {
            s->fields[s->field_count].is_array = 1;
            if (match(parser, TOKEN_NUMBER)) {
                s->fields[s->field_count].array_size = atoi(previous(parser).value);
            } else {
                fprintf(stderr, "Parse error at line %d, column %d: Expected array size\n", 
                        peek(parser).line, peek(parser).column);
                s->fields[s->field_count].array_size = 0;
            }
            consume(parser, TOKEN_RBRACKET, "Expected ']' after array size");
        }
        
        consume(parser, TOKEN_SEMICOLON, "Expected ';' after field declaration");
        s->field_count++;
    }
    
    consume(parser, TOKEN_RBRACE, "Expected '}' after struct fields");
    consume(parser, TOKEN_SEMICOLON, "Expected ';' after struct definition");
    return s;
}

// Parse function declaration
Function *parse_function(Parser *parser) {
    Function *func = create_function();
    if (match(parser, TOKEN_INT) || match(parser, TOKEN_FLOAT) || 
        match(parser, TOKEN_CHAR) || match(parser, TOKEN_VOID)) {
        func->return_type = token_to_var_type(previous(parser).type, parser);
    } else {
        fprintf(stderr, "Parse error at line %d, column %d: Expected return type\n", 
                peek(parser).line, peek(parser).column);
        func->return_type = TYPE_VOID;
    }
    
    consume(parser, TOKEN_ID, "Expected function name");
    func->name = strdup(previous(parser).value);
    consume(parser, TOKEN_LPAREN, "Expected '(' after function name");
    
    func->params = malloc(10 * sizeof(Variable));
    func->param_count = 0;
    int capacity = 10;
    
    if (!check(parser, TOKEN_RPAREN)) {
        do {
            if (func->param_count >= capacity) {
                capacity *= 2;
                func->params = realloc(func->params, capacity * sizeof(Variable));
            }
            if (match(parser, TOKEN_INT) || match(parser, TOKEN_FLOAT) || 
                match(parser, TOKEN_CHAR) || match(parser, TOKEN_VOID)) {
                func->params[func->param_count].type = token_to_var_type(previous(parser).type, parser);
            } else if (match(parser, TOKEN_STRUCT)) {
                consume(parser, TOKEN_ID, "Expected struct name");
                func->params[func->param_count].type = TYPE_VOID;
                func->params[func->param_count].struct_name = strdup(previous(parser).value);
            } else {
                fprintf(stderr, "Parse error at line %d, column %d: Expected parameter type\n", 
                        peek(parser).line, peek(parser).column);
                func->params[func->param_count].type = TYPE_INT;
            }
            
            consume(parser, TOKEN_ID, "Expected parameter name");
            func->params[func->param_count].name = strdup(previous(parser).value);
            func->params[func->param_count].is_array = 0;
            func->param_count++;
        } while (match(parser, TOKEN_COMMA));
    }
    
    consume(parser, TOKEN_RPAREN, "Expected ')' after parameters");
    func->body = parse_block(parser);
    return func;
}

// Parse statement
Statement *parse_statement(Parser *parser) {
    if (match(parser, TOKEN_IF)) {
        return parse_if_statement(parser);
    }
    if (match(parser, TOKEN_WHILE)) {
        return parse_while_statement(parser);
    }
    if (match(parser, TOKEN_FOR)) {
        return parse_for_statement(parser);
    }
    if (match(parser, TOKEN_RETURN)) {
        return parse_return_statement(parser);
    }
    if (match(parser, TOKEN_BREAK)) {
        return parse_break_statement(parser);
    }
    if (match(parser, TOKEN_CONTINUE)) {
        return parse_continue_statement(parser);
    }
    if (match(parser, TOKEN_PRINTF)) {
        return parse_print_statement(parser);
    }
    if (match(parser, TOKEN_LBRACE)) {
        parser->current--; // Backtrack to parse block
        return parse_block(parser);
    }
    if (match(parser, TOKEN_INT) || match(parser, TOKEN_FLOAT) || match(parser, TOKEN_CHAR)) {
        return parse_var_declaration(parser, token_to_var_type(previous(parser).type, parser), NULL);
    }
    if (match(parser, TOKEN_STRUCT)) {
        consume(parser, TOKEN_ID, "Expected struct name");
        char *struct_name = strdup(previous(parser).value);
        Statement *stmt = parse_var_declaration(parser, TYPE_VOID, struct_name);
        free(struct_name);
        return stmt;
    }
    return parse_expression_statement(parser);
}

// Main parsing function
Program *parse(Token *tokens, int token_count) {
    Parser parser = { tokens, token_count, 0 };
    program = malloc(sizeof(Program));
    program->functions = malloc(10 * sizeof(Function*));
    program->function_count = 0;
    program->global_vars = malloc(10 * sizeof(Variable));
    program->global_var_count = 0;
    program->structs = malloc(10 * sizeof(Struct));
    program->struct_count = 0;
    
    int func_capacity = 10;
    int var_capacity = 10;
    int struct_capacity = 10;
    
    while (!is_at_end(&parser)) {
        if (match(&parser, TOKEN_STRUCT)) {
            if (program->struct_count >= struct_capacity) {
                struct_capacity *= 2;
                program->structs = realloc(program->structs, struct_capacity * sizeof(Struct));
            }
            program->structs[program->struct_count++] = *parse_struct(&parser);
            continue;
        }
        
        if (match(&parser, TOKEN_INT) || match(&parser, TOKEN_FLOAT) || 
            match(&parser, TOKEN_CHAR) || match(&parser, TOKEN_VOID)) {
            TokenType type_token = previous(&parser).type;
            if (check(&parser, TOKEN_ID) && parser.tokens[parser.current + 1].type == TOKEN_LPAREN) {
                if (program->function_count >= func_capacity) {
                    func_capacity *= 2;
                    program->functions = realloc(program->functions, func_capacity * sizeof(Function*));
                }
                parser.current--; // Backtrack to parse function
                program->functions[program->function_count++] = parse_function(&parser);
            } else {
                if (program->global_var_count >= var_capacity) {
                    var_capacity *= 2;
                    program->global_vars = realloc(program->global_vars, var_capacity * sizeof(Variable));
                }
                parser.current--; // Backtrack to parse variable
                Statement *var_stmt = parse_var_declaration(&parser, token_to_var_type(type_token, &parser), NULL);
                program->global_vars[program->global_var_count] = var_stmt->var_decl.var;
                program->global_var_count++;
                free(var_stmt);
            }
        } else {
            fprintf(stderr, "Parse error at line %d, column %d: Unexpected token\n", 
                    peek(&parser).line, peek(&parser).column);
            advance(&parser);
        }
    }
    
    return program;
}

// Free program memory (simplified, assuming all pointers are allocated)
void free_program(Program *prog) {
    // Free structs
    for (int i = 0; i < prog->struct_count; i++) {
        free(prog->structs[i].name);
        for (int j = 0; j < prog->structs[i].field_count; j++) {
            free(prog->structs[i].fields[j].name);
            if (prog->structs[i].fields[j].struct_name) {
                free(prog->structs[i].fields[j].struct_name);
            }
        }
        free(prog->structs[i].fields);
    }
    free(prog->structs);
    
    // Free global variables
    for (int i = 0; i < prog->global_var_count; i++) {
        free(prog->global_vars[i].name);
        if (prog->global_vars[i].struct_name) {
            free(prog->global_vars[i].struct_name);
        }
    }
    free(prog->global_vars);
    
    // Free functions (simplified, should free expressions and statements recursively)
    for (int i = 0; i < prog->function_count; i++) {
        free(prog->functions[i]->name);
        for (int j = 0; j < prog->functions[i]->param_count; j++) {
            free(prog->functions[i]->params[j].name);
        }
        free(prog->functions[i]->params);
        // Free body statements (not implemented for brevity)
        free(prog->functions[i]);
    }
    free(prog->functions);
    
    free(prog);
}