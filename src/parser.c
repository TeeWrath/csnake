#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
    return expr;
}

Statement *create_statement() {
    Statement *stmt = malloc(sizeof(Statement));
    if (!stmt) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
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

// Parser state
typedef struct {
    Token *tokens;
    int token_count;
    int current;
} Parser;

// Helper function prototypes
VariableType token_to_var_type(TokenType type);
Expression *parse_expression(Parser *parser);
Statement *parse_statement(Parser *parser);
Statement *parse_block(Parser *parser);
Function *parse_function(Parser *parser);
void advance(Parser *parser);
Token peek(Parser *parser);
Token previous(Parser *parser);
int match(Parser *parser, TokenType type);
int check(Parser *parser, TokenType type);
void consume(Parser *parser, TokenType type, const char *message);
int is_at_end(Parser *parser);
void synchronize(Parser *parser);

// Convert token type to variable type
VariableType token_to_var_type(TokenType type) {
    switch (type) {
        case TOKEN_INT: return TYPE_INT;
        case TOKEN_FLOAT: return TYPE_FLOAT;
        case TOKEN_CHAR: return TYPE_CHAR;
        case TOKEN_VOID: return TYPE_VOID;
        default: 
            fprintf(stderr, "Error: Unknown type\n");
            return TYPE_INT; // Default to int
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
    advance(parser);
    
    while (!is_at_end(parser)) {
        if (previous(parser).type == TOKEN_SEMICOLON) return;
        
        switch (peek(parser).type) {
            case TOKEN_INT:
            case TOKEN_FLOAT:
            case TOKEN_CHAR:
            case TOKEN_VOID:
            case TOKEN_IF:
            case TOKEN_WHILE:
            case TOKEN_FOR:
            case TOKEN_RETURN:
                return;
            default:
                break;
        }
        
        advance(parser);
    }
}

// Parse primary expression (literals, variables, parenthesized expressions)
Expression *parse_primary(Parser *parser) {
    Expression *expr = create_expression();
    
    if (match(parser, TOKEN_NUMBER)) {
        expr->type = EXPR_LITERAL;
        expr->literal.lit_type = TYPE_INT;
        
        // Check if it's a float
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
            expr->call.args = malloc(10 * sizeof(Expression*)); // Start with space for 10 args
            expr->call.arg_count = 0;
            
            // Parse arguments
            if (!check(parser, TOKEN_RPAREN)) {
                do {
                    // Resize if needed
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
        return expr;
    }
    
    if (match(parser, TOKEN_LPAREN)) {
        Expression *grouping = parse_expression(parser);
        consume(parser, TOKEN_RPAREN, "Expected ')' after expression");
        free(expr); // Free the unused expr
        return grouping;
    }
    
    fprintf(stderr, "Parse error at line %d, column %d: Expected expression\n", 
            peek(parser).line, peek(parser).column);
    return expr; // Return something to avoid crashes
}

// Parse unary expressions
Expression *parse_unary(Parser *parser) {
    if (match(parser, TOKEN_MINUS) || match(parser, TOKEN_NOT) || 
        match(parser, TOKEN_INCR) || match(parser, TOKEN_DECR)) {
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
        }
        
        expr->unary.expr = parse_unary(parser); // Parse the operand
        return expr;
    }
    
    Expression *expr = parse_primary(parser);
    
    // Handle postfix operators
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

// Parse term expressions (+, -)
Expression *parse_term(Parser *parser) {
    Expression *expr = parse_factor(parser);
    
    while (match(parser, TOKEN_PLUS) || match(parser, TOKEN_MINUS)) {
        Expression *binary = create_expression();
        binary->type = EXPR_BINARY;
        
        if (previous(parser).type == TOKEN_PLUS) {
            binary->binary.op = OP_ADD;
        } else if (previous(parser).type == TOKEN_MINUS) {
            binary->binary.op = OP_SUB;
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

// Parse logical AND expressions (&&)
Expression *parse_and(Parser *parser) {
    Expression *expr = parse_equality(parser);
    
    while (match(parser, TOKEN_AND)) {
        Expression *binary = create_expression();
        binary->type = EXPR_BINARY;
        binary->binary.op = OP_AND;
        binary->binary.left = expr;
        binary->binary.right = parse_equality(parser);
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
        
        // Check if left is a valid lvalue (variable or array access)
        if (expr->type != EXPR_VARIABLE && expr->type != EXPR_ARRAY_ACCESS) {
            fprintf(stderr, "Parse error at line %d, column %d: Invalid assignment target\n", 
                    peek(parser).line, peek(parser).column);
        }
        
        binary->binary.left = expr;
        binary->binary.right = parse_assignment(parser); // Right-associative
        return binary;
    }
    
    return expr;
}

// The main expression parsing function
Expression *parse_expression(Parser *parser) {
    return parse_assignment(parser);
}

// Parse variable declaration
Statement *parse_var_declaration(Parser *parser, VariableType type) {
    Statement *stmt = create_statement();
    stmt->type = STMT_VAR_DECL;
    
    // Get variable name
    consume(parser, TOKEN_ID, "Expected variable name");
    stmt->var_decl.var.name = strdup(previous(parser).value);
    stmt->var_decl.var.type = type;
    stmt->var_decl.var.is_initialized = 0;
    stmt->var_decl.var.is_array = 0;
    
    // Check for array declaration
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
    
    // Check for initialization
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
    
    // Parse format string
    if (match(parser, TOKEN_STRING)) {
        stmt->print.format = strdup(previous(parser).value);
    } else {
        fprintf(stderr, "Parse error at line %d, column %d: Expected format string\n", 
                peek(parser).line, peek(parser).column);
        stmt->print.format = strdup("");
    }
    
    // Parse arguments if any
    stmt->print.args = malloc(10 * sizeof(Expression*));
    stmt->print.arg_count = 0;
    
    while (match(parser, TOKEN_COMMA)) {
        if (stmt->print.arg_count >= 10) {
            stmt->print.args = realloc(stmt->print.args, (stmt->print.arg_count + 10) * sizeof(Expression*));
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
        
        stmt->block.statements[stmt->block.stmt_count++] = parse_statement(parser);
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
    
    // Parse initializer
    if (match(parser, TOKEN_INT) || match(parser, TOKEN_FLOAT) || match(parser, TOKEN_CHAR)) {
        TokenType type_token = previous(parser).type;
        stmt->for_stmt.initializer = parse_var_declaration(parser, token_to_var_type(type_token));
    } else if (match(parser, TOKEN_SEMICOLON)) {
        stmt->for_stmt.initializer = NULL;
    } else {
        stmt->for_stmt.initializer = parse_expression_statement(parser);
    }
    
    // Parse condition
    if (!check(parser, TOKEN_SEMICOLON)) {
        stmt->for_stmt.condition = parse_expression(parser);
    } else {
        // No condition provided, default to true
        stmt->for_stmt.condition = create_expression();
        stmt->for_stmt.condition->type = EXPR_LITERAL;
        stmt->for_stmt.condition->literal.lit_type = TYPE_INT;
        stmt->for_stmt.condition->literal.int_val = 1;
    }
    consume(parser, TOKEN_SEMICOLON, "Expected ';' after for condition");
    
    // Parse increment
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

// The main statement parsing function
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
    
    if (match(parser, TOKEN_LBRACE)) {
        parser->current--; // Go back to the '{'
        return parse_block(parser);
    }
    
    if (match(parser, TOKEN_PRINTF)) {
        return parse_print_statement(parser);
    }
    
    if (match(parser, TOKEN_INT) || match(parser, TOKEN_FLOAT) || 
        match(parser, TOKEN_CHAR) || match(parser, TOKEN_VOID)) {
        TokenType type_token = previous(parser).type;
        
        // Check if it's a function declaration
        if (check(parser, TOKEN_ID)) {
            Token id_token = peek(parser);
            advance(parser);
            
            if (check(parser, TOKEN_LPAREN)) {
                parser->current -= 2; // Go back to the type token
                return NULL; // Signal to parse_function
            }
            
            // It's a variable declaration, go back
            parser->current--;
        }
        
        return parse_var_declaration(parser, token_to_var_type(type_token));
    }
    
    return parse_expression_statement(parser);
}

// Parse function declaration
Function *parse_function(Parser *parser) {
    Function *func = create_function();
    
    // Parse return type
    if (match(parser, TOKEN_INT) || match(parser, TOKEN_FLOAT) || 
        match(parser, TOKEN_CHAR) || match(parser, TOKEN_VOID)) {
        func->return_type = token_to_var_type(previous(parser).type);
    } else {
        fprintf(stderr, "Parse error at line %d, column %d: Expected function return type\n", 
                peek(parser).line, peek(parser).column);
        func->return_type = TYPE_VOID;
    }
    
    // Parse function name
    consume(parser, TOKEN_ID, "Expected function name");
    func->name = strdup(previous(parser).value);
    
    // Parse parameters
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
            
            // Parse parameter type
            if (match(parser, TOKEN_INT) || match(parser, TOKEN_FLOAT) || 
                match(parser, TOKEN_CHAR) || match(parser, TOKEN_VOID)) {
                func->params[func->param_count].type = token_to_var_type(previous(parser).type);
            } else {
                fprintf(stderr, "Parse error at line %d, column %d: Expected parameter type\n", 
                        peek(parser).line, peek(parser).column);
                func->params[func->param_count].type = TYPE_INT;
            }
            
            // Parse parameter name
            consume(parser, TOKEN_ID, "Expected parameter name");
            func->params[func->param_count].name = strdup(previous(parser).value);
            
            // Check for array parameter
            func->params[func->param_count].is_array = 0;
            if (match(parser, TOKEN_LBRACKET)) {
                func->params[func->param_count].is_array = 1;
                consume(parser, TOKEN_RBRACKET, "Expected ']' after array parameter");
            }
            
            func->param_count++;
        } while (match(parser, TOKEN_COMMA));
    }
    
    consume(parser, TOKEN_RPAREN, "Expected ')' after parameters");
    
    // Parse function body
    func->body = parse_block(parser);
    
    return func;
}

// The main parsing function
Program *parse(Token *tokens, int token_count) {
    Parser parser;
    parser.tokens = tokens;
    parser.token_count = token_count;
    parser.current = 0;
    
    program = malloc(sizeof(Program));
    program->functions = malloc(10 * sizeof(Function*));
    program->function_count = 0;
    program->global_vars = malloc(10 * sizeof(Variable));
    program->global_var_count = 0;
    
    int func_capacity = 10;
    int var_capacity = 10;
    
    while (!is_at_end(&parser)) {
        // Try to parse a function
        Function *func = parse_function(&parser);
        
        if (func != NULL) {
            if (program->function_count >= func_capacity) {
                func_capacity *= 2;
                program->functions = realloc(program->functions, func_capacity * sizeof(Function*));
            }
            program->functions[program->function_count++] = func;
        } else {
            // Not a function, try parsing a global variable
            Statement *stmt = parse_statement(&parser);
            
            if (stmt != NULL && stmt->type == STMT_VAR_DECL) {
                if (program->global_var_count >= var_capacity) {
                    var_capacity *= 2;
                    program->global_vars = realloc(program->global_vars, var_capacity * sizeof(Variable));
                }
                program->global_vars[program->global_var_count++] = stmt->var_decl.var;
                free(stmt); // We've extracted the variable, free the statement
            } else if (stmt != NULL) {
                fprintf(stderr, "Error: Only function declarations and global variables are allowed at the top level\n");
                free(stmt);
            }
        }
    }
    
    return program;
}

// Free the memory allocated for a program
void free_program(Program *program) {
    if (program == NULL) return;
    
    // Free functions
    for (int i = 0; i < program->function_count; i++) {
        free(program->functions[i]->name);
        for (int j = 0; j < program->functions[i]->param_count; j++) {
            free(program->functions[i]->params[j].name);
        }
        free(program->functions[i]->params);
        // TODO: Free function body (recursively free statements and expressions)
        free(program->functions[i]);
    }
    free(program->functions);
    
    // Free global variables
    for (int i = 0; i < program->global_var_count; i++) {
        free(program->global_vars[i].name);
    }
    free(program->global_vars);
    
    free(program);
}