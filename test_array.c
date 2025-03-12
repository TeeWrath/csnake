// Test arrays and loops in C
int find_max(int arr[], int size) {
    int max = arr[0];
    for (int i = 1; i < size; i++) {
        if (arr[i] > max) {
            max = arr[i];
        }
    }
    return max;
}

int find_min(int arr[], int size) {
    int min = arr[0];
    int i = 1;
    while (i < size) {
        if (arr[i] < min) {
            min = arr[i];
        }
        i++;
    }
    return min;
}

float calculate_average(int arr[], int size) {
    int sum = 0;
    for (int i = 0; i < size; i++) {
        sum += arr[i];
    }
    return (float)sum / size;
}

int main() {
    int numbers[10] = {23, 45, 12, 67, 89, 34, 56, 78, 90, 32};
    int size = 10;
    
    int max_value = find_max(numbers, size);
    int min_value = find_min(numbers, size);
    float avg_value = calculate_average(numbers, size);
    
    printf("Array values: ");
    for (int i = 0; i < size; i++) {
        printf("%d ", numbers[i]);
    }
    printf("\n");
    
    printf("Maximum value: %d\n", max_value);
    printf("Minimum value: %d\n", min_value);
    printf("Average value: %f\n", avg_value);
    
    return 0;
}