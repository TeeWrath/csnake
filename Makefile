CC = gcc
CFLAGS = -Iinclude -Wall -Wextra -g
SRC = src/main.c src/lexer.c src/parser.c src/codegen.c src/struct_codegen.c
OBJ = $(SRC:.c=.o)
TARGET = csnakecompiler

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET) output.py struct_test.py bitwise_test.py asm_test.py

# Test target to run the compiler on a sample C file
test: $(TARGET)
	@echo "Testing with sample factorial program..."
	@cat test_factorial.c
	./$(TARGET) test_factorial.c
	@echo "\nOutput Python code:"
	@cat output.py
	@echo "\nRunning Python code:"
	python3 output.py

# Test target for structs and static typing
struct_test: $(TARGET)
	@echo "Testing with struct and static typing program..."
	@cat test_struct.c
	./$(TARGET) test_struct.c -o struct_test.py
	@echo "\nGenerated Python code:"
	@cat struct_test.py
	@echo "\nRunning Python code:"
	python3 struct_test.py

# Test target for bitwise operations
bitwise_test: $(TARGET)
	@echo "Testing bitwise operations..."
	@echo "// Bitwise test" > bitwise_test.c
	@echo "int main() {" >> bitwise_test.c
	@echo "    unsigned int flags = 0;" >> bitwise_test.c
	@echo "    flags |= 0x1;" >> bitwise_test.c
	@echo "    flags ^= 0x6;" >> bitwise_test.c
	@echo "    int shifted = flags << 2;" >> bitwise_test.c
	@echo "    int masked = flags & 0xFF;" >> bitwise_test.c
	@echo "    return 0;" >> bitwise_test.c
	@echo "}" >> bitwise_test.c
	./$(TARGET) bitwise_test.c -o bitwise_test.py
	@echo "\nGenerated Python code:"
	@cat bitwise_test.py
	@echo "\nRunning Python code:"
	python3 bitwise_test.py

# Test target for inline assembly
asm_test: $(TARGET)
	@echo "Testing inline assembly support..."
	@cat test_asm.c
	./$(TARGET) test_asm.c -o asm_test.py
	@echo "\nGenerated Python code:"
	@cat asm_test.py
	@echo "\nRunning Python code:"
	python3 asm_test.py

# Create a sample test file if it doesn't exist
test_factorial.c:
	@echo "Creating sample test file: test_factorial.c"
	@echo "// Calculate factorial in C" > test_factorial.c
	@echo "int factorial(int n) {" >> test_factorial.c
	@echo "    if (n <= 1) {" >> test_factorial.c
	@echo "        return 1;" >> test_factorial.c
	@echo "    }" >> test_factorial.c
	@echo "    return n * factorial(n - 1);" >> test_factorial.c
	@echo "}" >> test_factorial.c
	@echo "" >> test_factorial.c
	@echo "int main() {" >> test_factorial.c
	@echo "    int num = 5;" >> test_factorial.c
	@echo "    printf(\"Factorial of %d is %d\\n\", num, factorial(num));" >> test_factorial.c
	@echo "    return 0;" >> test_factorial.c
	@echo "}" >> test_factorial.c

# Create a sample test file for inline assembly
test_asm.c:
	@echo "Creating sample test file: test_asm.c"
	@echo "// Test inline assembly" > test_asm.c
	@echo "int main() {" >> test_asm.c
	@echo "    int result = 0;" >> test_asm.c
	@echo "    asm(\"movl \$42, %eax\");" >> test_asm.c
	@echo "    asm(\"movl %0, %1\" : \"=r\"(result) : \"r\"(result) : \"eax\");" >> test_asm.c
	@echo "    printf(\"Result: %d\\n\", result);" >> test_asm.c
	@echo "    return 0;" >> test_asm.c
	@echo "}" >> test_asm.c

.PHONY: all clean test struct_test bitwise_test asm_test