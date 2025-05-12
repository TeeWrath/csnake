from dataclasses import dataclass, field
from typing import List

# Struct definitions
@dataclass
class Point:
    x: int = 0
    y: float = 0.0

# Global variables

# Functions
def main() -> int:
    p: Point = Point()
    p.x = 10
    p.y = 20.500000
    print(f"Point: ({p.x}, {p.y})\n")
    return 0

if __name__ == "__main__":
    main()
