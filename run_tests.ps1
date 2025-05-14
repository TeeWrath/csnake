# # Run make clean
# Write-Host "Running make clean..."
# make clean
# if ($LASTEXITCODE -ne 0) {
#     Write-Host "Error: make clean failed."
#     exit 1
# }

# # Run make
# Write-Host "Running make..."
# make
# if ($LASTEXITCODE -ne 0) {
#     Write-Host "Error: make failed."
#     exit 1
# }

# Ensure csnakecompiler exists
if (-not (Test-Path "./csnakecompiler")) {
    Write-Host "Error: csnakecompiler not found in current directory."
    exit 1
}

# Create test_result folder if it doesn't exist
if (-not (Test-Path "test_result")) {
    Write-Host "Creating test_result folder..."
    New-Item -ItemType Directory -Path "test_result" | Out-Null
}

# Check if test folder exists
if (-not (Test-Path "test")) {
    Write-Host "Error: test folder not found."
    exit 1
}

# Find all .c files in the test folder
$c_files = Get-ChildItem -Path "test" -Recurse -Include "*.c"

# Check if there are any .c files
if ($c_files.Count -eq 0) {
    Write-Host "Error: No .c files found in test folder."
    exit 1
}

# Process each .c file
foreach ($c_file in $c_files) {
    # Extract the base name (e.g., test_struct from test\test_struct.c)
    $base_name = [System.IO.Path]::GetFileNameWithoutExtension($c_file.Name)
    Write-Host "Processing $($c_file.FullName)..."

    # Define output Python file path
    $py_file = "test_result\$base_name.py"
    # Define output text file path for Python execution result
    $output_file = "test_result\$base_name.txt"

    # Run csnakecompiler to generate Python file
    ./csnakecompiler "$($c_file.FullName)" -o "$py_file"
    if ($LASTEXITCODE -ne 0) {
        Write-Host "Error: Failed to compile $($c_file.FullName)"
        continue
    }

    # Check if Python file was generated
    if (-not (Test-Path "$py_file")) {
        Write-Host "Error: Python file $py_file not generated."
        continue
    }

    # Run the generated Python file and capture output
    Write-Host "Running $py_file..."
    try {
        $output = python3 "$py_file" 2>&1
        $output | Out-File -FilePath "$output_file" -Encoding utf8
        if ($LASTEXITCODE -ne 0) {
            Write-Host "Error: Failed to run $py_file. See $output_file for details."
        } else {
            Write-Host "Output saved to $output_file"
        }
    } catch {
        Write-Host "Error: Failed to run $py_file. See $output_file for details."
        $_ | Out-File -FilePath "$output_file" -Encoding utf8
    }
}

Write-Host "All tests processed. Results are in test_result folder."