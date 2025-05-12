#!/bin/bash

# Ensure csnakecompiler exists and is executable
if [ ! -f "./csnakecompiler" ]; then
    echo "Error: csnakecompiler not found in current directory."
    exit 1
fi

if [ ! -x "./csnakecompiler" ]; then
    echo "Error: csnakecompiler is not executable. Adding execute permission..."
    chmod +x ./csnakecompiler
fi

# Create test_result folder if it doesn't exist
if [ ! -d "test_result" ]; then
    echo "Creating test_result folder..."
    mkdir test_result
fi

# Check if test folder exists
if [ ! -d "test" ]; then
    echo "Error: test folder not found."
    exit 1
fi

# Find all .c files in the test folder
c_files=$(find test -type f -name "*.c")

# Check if there are any .c files
if [ -z "$c_files" ]; then
    echo "Error: No .c files found in test folder."
    exit 1
fi

# Process each .c file
for c_file in $c_files; do
    # Extract the base name (e.g., test_struct from test/test_struct.c)
    base_name=$(basename "$c_file" .c)
    echo "Processing $c_file..."

    # Define output Python file path
    py_file="test_result/$base_name.py"
    # Define output text file path for Python execution result
    output_file="test_result/$base_name.txt"

    # Run csnakecompiler to generate Python file
    ./csnakecompiler "$c_file" -o "$py_file"
    if [ $? -ne 0 ]; then
        echo "Error: Failed to compile $c_file"
        continue
    fi

    # Check if Python file was generated
    if [ ! -f "$py_file" ]; then
        echo "Error: Python file $py_file not generated."
        continue
    fi

    # Run the generated Python file and capture output
    echo "Running $py_file..."
    python3 "$py_file" > "$output_file" 2>&1
    if [ $? -ne 0 ]; then
        echo "Error: Failed to run $py_file. See $output_file for details."
    else
        echo "Output saved to $output_file"
    fi
done

echo "All tests processed. Results are in test_result folder."