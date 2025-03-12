// Test arrays and loops in C with simplified syntax
int find_max(int arr[], int size) {
    int max = arr[0];
    int i = 1;
    while (i < size) {
        if (arr[i] > max) {
            max = arr[i];
        }
        i = i + 1;
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
        i = i + 1;
    }
    return min;
}

int calculate_sum(int arr[], int size) {
    int sum = 0;
    int i = 0;
    while (i < size) {
        sum = sum + arr[i];
        i = i + 1;
    }
    return sum;
}

float calculate_average(int arr[], int size) {
    int sum = calculate_sum(arr, size);
    float average = sum;
    average = average / size;
    return average;
}

int main() {
    int numbers[10];
    numbers[0] = 23;
    numbers[1] = 45;
    numbers[2] = 12;
    numbers[3] = 67;
    numbers[4] = 89;
    numbers[5] = 34;
    numbers[6] = 56;
    numbers[7] = 78;
    numbers[8] = 90;
    numbers[9] = 32;
    
    int size = 10;
    
    int max_value = find_max(numbers, size);
    int min_value = find_min(numbers, size);
    float avg_value = calculate_average(numbers, size);
    
    printf("Maximum value: %d\n", max_value);
    printf("Minimum value: %d\n", min_value);
    printf("Average value: %f\n", avg_value);
    
    return 0;
}