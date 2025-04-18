CC = gcc
CFLAGS = -Iinclude -Wall -Wextra -g
SRC = src/main.c src/lexer.c src/parser.c src/codegen.c
OBJ = $(SRC:.c=.o)
TARGET = csnakecompiler

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET) output.py

# Test target to run the compiler on a sample C file
test: $(TARGET)
	@echo "Testing with sample factorial program..."
	@cat test_factorial.c
	./$(TARGET) test_factorial.c
	@echo "\nOutput Python code:"
	@cat output.py
	@echo "\nRunning Python code:"
	python3 output.py

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

.PHONY: all clean test