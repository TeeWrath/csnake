// Calculate the sum of first n natural numbers
int sum_iterative(int n) {
    int result = 0;
    for (int i = 1; i <= n; i++) {
        result += i;
    }
    return result;
}

// Calculate the sum of first n natural numbers using while loop
int sum_while(int n) {
    int result = 0;
    int i = 1;
    while (i <= n) {
        result += i;
        i++;
    }
    return result;
}

int main() {
    int n = 10;
    
    // Test for loop
    int for_result = sum_iterative(n);
    printf("Sum of first %d numbers using for loop: %d\n", n, for_result);
    
    // Test while loop
    int while_result = sum_while(n);
    printf("Sum of first %d numbers using while loop: %d\n", n, while_result);
    
    // Test if-else
    if (for_result == while_result) {
        printf("Both methods give the same result!\n");
    } else {
        printf("There's a bug in one of the implementations.\n");
    }
    
    return 0;
}