from dataclasses import dataclass, field
from typing import List

# Global variables

# Functions
def factorial(n: int) -> int:
    if (n <= 1):
        return 1
    return (n * factorial((n - 1)))

def main() -> int:
    num: int = 5
    print(f"Factorial of {num} is {factorial(num)}\n")
    return 0

if __name__ == "__main__":
    main()
