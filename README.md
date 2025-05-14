````markdown
# Csnake: C to Python Transpiler

Csnake is a cross-code translator designed to transpile C language code into Python, facilitating seamless code migration and interoperability between these two popular programming languages.

## Features

- **Code Transpilation**: Converts C code structures and syntax into equivalent Python code.
- **Support for Control Structures**: Handles if-else conditions, for loops, while loops, and function calls.
- **Variable Management**: Converts C variable declarations into Python assignments.
- **Function Translation**: Preserves function logic across languages.
- **Automation**: Streamlines the process of translating C projects to Python, reducing manual effort.
- **Integration**: Eases integration of existing C codebases into Python environments.

## Supported C Features

- Variable declarations with initialization
- Basic data types (int, float, char)
- Arrays
- Arithmetic, logical, and comparison operators
- Control structures (if-else, for, while)
- Function declarations and calls
- Recursive functions
- Printf statements (converted to Python's print)

## Getting Started

To utilize Csnake for transpiling your C code to Python, follow these steps:

1. **Clone the Repository**:

   ```bash
   git clone https://github.com/YourUsername/csnake.git
   ```

2. **Navigate to the Project Directory**:

   ```bash
   cd csnake
   ```

3. **Build the Project**:

   Use the provided `Makefile` to compile the necessary components.

   ```bash
   make
   ```

4. **Transpile Your C Code**:

   Run the `csnakecompiler` with your C source file as input to generate the corresponding Python code.

   ```bash
   ./csnakecompiler path/to/your_code.c -o output.py
   ```

5. **Run the Generated Python Code**:

   ```bash
   python3 output.py
   ```

## Quick Test

You can quickly test the compiler using the built-in test files:

```bash
make test
```

This will compile the transpiler, create a sample factorial calculation program in C, compile it to Python, and run the resulting Python code.

## Run Instructions

To run all automated tests:

* **On Windows**:

  1. Open **PowerShell** in the project root directory.
  2. Run the following command:

     ```powershell
     .\run_tests.ps1
     ```

* **On Linux**:

  1. Open **Terminal** in the project root directory.
  2. Run the following command:

     ```bash
     ./run_tests.sh
     ```

## Project Structure

* `include/`: Contains header files used in the project.

  * `lexer.h`: Defines token types and lexer function prototypes.
  * `parser.h`: Defines the AST structures and parser function prototypes.
  * `codegen.h`: Defines code generation function prototypes.

* `src/`: Holds the source code for Csnake's implementation.

  * `lexer.c`: Tokenizes C code into language tokens.
  * `parser.c`: Parses tokens into an Abstract Syntax Tree (AST).
  * `codegen.c`: Generates Python code from the AST.
  * `main.c`: Main program that ties everything together.

* `test_factorial.c`: Sample C program for testing factorial calculation.

* `test_loop.c`: Sample C program for testing loops and conditionals.

* `Makefile`: Defines the build instructions for the project.

## Examples

### Example 1: Factorial Calculation

**Input (C code):**

```c
// Calculate factorial in C
int factorial(int n) {
    if (n <= 1) {
        return 1;
    }
    return n * factorial(n - 1);
}

int main() {
    int num = 5;
    printf("Factorial of %d is %d\n", num, factorial(num));
    return 0;
}
```

**Output (Python code):**

```python
# Generated Python code from C source
# This file was automatically translated by Csnake

def factorial(n):
    if (n <= 1):
        return 1
    return (n * factorial((n - 1)))

def main():
    num = 5
    print(f"Factorial of {num} is {factorial(num)}")

# Call main function if this script is run directly
if __name__ == "__main__":
    main()
```

### Example 2: Loops and Conditionals

**Input (C code):**

```c
// Calculate the sum of first n natural numbers
int sum_iterative(int n) {
    int result = 0;
    for (int i = 1; i <= n; i++) {
        result += i;
    }
    return result;
}

int main() {
    int n = 10;
    int result = sum_iterative(n);
    printf("Sum of first %d numbers is %d\n", n, result);
    return 0;
}
```

**Output (Python code):**

```python
# Generated Python code from C source
# This file was automatically translated by Csnake

def sum_iterative(n):
    result = 0
    i = 1
    while (i <= n):
        result += i
        i += 1
    return result

def main():
    n = 10
    result = sum_iterative(n)
    print(f"Sum of first {n} numbers is {result}")

# Call main function if this script is run directly
if __name__ == "__main__":
    main()
```

## Limitations

* Complex C features like structs, pointers, and memory management are not fully supported.
* Some C-specific operators and features may not have perfect Python equivalents.
* The transpiler handles basic printf formats but complex formats may not translate perfectly.

## Contributing

Contributions are welcome! If you'd like to improve Csnake or report issues, please follow these steps:

1. **Fork the Repository**: Click on the 'Fork' button at the top right corner of this page.
2. **Create a New Branch**: Use a descriptive name for your branch.
3. **Make Your Changes**: Implement your feature or fix.
4. **Submit a Pull Request**: Provide a clear description of your changes.

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.

```