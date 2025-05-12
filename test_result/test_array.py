from dataclasses import dataclass, field
from typing import List

# Global variables

# Functions
def find_max(arr: int, size: int) -> int:
    max: int = arr[0]
    i: int = 1
    while (i < size):
        if (arr[i] > max):
            max = arr[i]
        i = (i + 1)
    return max

def find_min(arr: int, size: int) -> int:
    min: int = arr[0]
    i: int = 1
    while (i < size):
        if (arr[i] < min):
            min = arr[i]
        i = (i + 1)
    return min

def calculate_sum(arr: int, size: int) -> int:
    sum: int = 0
    i: int = 0
    while (i < size):
        sum = (sum + arr[i])
        i = (i + 1)
    return sum

def calculate_average(arr: int, size: int) -> float:
    sum: int = calculate_sum(arr, size)
    average: float = sum
    average = (average / size)
    return average

def main() -> int:
    numbers: int = [0] * 10
    numbers[0] = 23
    numbers[1] = 45
    numbers[2] = 12
    numbers[3] = 67
    numbers[4] = 89
    numbers[5] = 34
    numbers[6] = 56
    numbers[7] = 78
    numbers[8] = 90
    numbers[9] = 32
    size: int = 10
    max_value: int = find_max(numbers, size)
    min_value: int = find_min(numbers, size)
    avg_value: float = calculate_average(numbers, size)
    print(f"Maximum value: {max_value}\n")
    print(f"Minimum value: {min_value}\n")
    print(f"Average value: {avg_value}\n")
    return 0

if __name__ == "__main__":
    main()
