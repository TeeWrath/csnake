# Csnake

Csnake is a cross-code translator designed to transpile C language code into Python, facilitating seamless code migration and interoperability between these two popular programming languages.

## Features

- **Code Transpilation**: Converts C code structures and syntax into equivalent Python code.
- **Automation**: Streamlines the process of translating C projects to Python, reducing manual effort.
- **Integration**: Eases integration of existing C codebases into Python environments.

## Getting Started

To utilize Csnake for transpiling your C code to Python, follow these steps:

1. **Clone the Repository**:

   ```bash
   git clone https://github.com/TeeWrath/csnake.git
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
   ./csnakecompiler path/to/your_code.c
   ```


## Project Structure

- `include/`: Contains header files used in the project.
- `src/`: Holds the source code for Csnake's implementation.
- `.gitignore`: Specifies files and directories to be ignored by Git.
- `Makefile`: Defines the build instructions for the project.
- `csnakecompiler`: Executable for the C to Python code translation.

## Contributing

Contributions are welcome! If you'd like to improve Csnake or report issues, please follow these steps:

1. **Fork the Repository**: Click on the 'Fork' button at the top right corner of this page.
2. **Create a New Branch**: Use a descriptive name for your branch.
3. **Make Your Changes**: Implement your feature or fix.
4. **Submit a Pull Request**: Provide a clear description of your changes.

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.
