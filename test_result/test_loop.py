from dataclasses import dataclass, field
from typing import List

# Global variables

# Functions
def sum_iterative(n: int) -> int:
    result: int = 0
    i: int = 1
    while (i <= n):
        result = (result + i)
        i = (i + 1)
    return result

def sum_while(n: int) -> int:
    result: int = 0
    i: int = 1
    while (i <= n):
        result = (result + i)
        i = (i + 1)
    return result

def main() -> int:
    n: int = 10
    for_result: int = sum_iterative(n)
    print(f"Sum of first {n} numbers using iterative method: {for_result}\n")
    while_result: int = sum_while(n)
    print(f"Sum of first {n} numbers using while loop: {while_result}\n")
    if (for_result == while_result):
        print(f"Both methods give the same result!\n")
    else:
        print(f"There's a bug in one of the implementations.\n")
    return 0

if __name__ == "__main__":
    main()
