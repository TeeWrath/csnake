from dataclasses import dataclass
from typing import List

def main() -> int:
    x: int
    x = 42
        # Inline assembly block
    # Instructions: "mov %0, %%eax"
    # Outputs:
    #   "=r" (x)
    # Inputs:
    #   "r" (x)
    # Clobbers: 

    print(f"Value after asm: %d\n", x)
    return 0

if __name__ == "__main__":
    main()
