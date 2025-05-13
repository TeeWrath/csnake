#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/lexer.h"
#include "../include/parser.h"
#include "../include/codegen.h"

// Read entire file into a string
char *read_file(const char *filename) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        fprintf(stderr, "Error: Unable to open file '%s'\n", filename);
        return NULL;
    }
    
    // Get file size
    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    
    // Allocate buffer for file contents (+1 for null terminator)
    char *buffer = malloc(size + 1);
    if (!buffer) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        fclose(fp);
        return NULL;
    }
    
    // Read file contents
    size_t bytes_read = fread(buffer, 1, size, fp);
    if (bytes_read < (size_t)size) {
        fprintf(stderr, "Warning: Only read %zu of %ld bytes from file\n", bytes_read, size);
    }
    
    buffer[bytes_read] = '\0'; // Null-terminate the string
    fclose(fp);
    return buffer;
}

int main(int argc, char *argv[]) {
    const char *input_file = NULL;
    const char *output_file = "output.py";
    
    // Parse command line arguments
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-o") == 0 && i + 1 < argc) {
            output_file = argv[++i];
        } else if (input_file == NULL) {
            input_file = argv[i];
        } else {
            fprintf(stderr, "Error: Unexpected argument '%s'\n", argv[i]);
            return 1;
        }
    }
    
    if (input_file == NULL) {
        printf("Usage: %s <input_file.c> [-o output_file.py]\n", argv[0]);
        printf("Using built-in example code...\n");
        
        // Use a built-in example if no input file is provided
        const char *example = 
            "// Calculate factorial in C\n"
            "int factorial(int n) {\n"
            "    if (n <= 1) {\n"
            "        return 1;\n"
            "    }\n"
            "    return n * factorial(n - 1);\n"
            "}\n"
            "\n"
            "int main() {\n"
            "    int num = 5;\n"
            "    printf(\"Factorial of %d is %d\\n\", num, factorial(num));\n"
            "    return 0;\n"
            "}\n";
        
        printf("Example code:\n%s\n", example);
        
        // Tokenize input
        int token_count = 0;
        Token *tokens = lexer(example, &token_count);
        
        // Print tokens for debugging
        printf("Tokens:\n");
        for (int i = 0; i < token_count; i++) {
            print_token(tokens[i]);
        }
        
        // Parse tokens
        Program *program = parse(tokens, token_count);
        
        // Generate Python code
        generate_code(program, output_file);
        
        // Cleanup
        for (int i = 0; i < token_count; i++) {
            free(tokens[i].value);
        }
        free(tokens);
        free_program(program);
        
        return 0;
    }
    
    // Read input file
    char *input = read_file(input_file);
    if (!input) {
        return 1;
    }
    
    printf("Processing file: %s\n", input_file);
    
    // Tokenize input
    int token_count = 0;
    Token *tokens = lexer(input, &token_count);
    
    // Parse tokens
    Program *program = parse(tokens, token_count);
    
    // Generate Python code
    generate_code(program, output_file);
    
    // Cleanup
    free(input);
    for (int i = 0; i < token_count; i++) {
        free(tokens[i].value);
    }
    free(tokens);
    free_program(program);
    
    return 0;
}