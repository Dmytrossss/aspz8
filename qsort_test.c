#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

int int_compare(const void *a, const void *b) {
    return (*(int*)a - *(int*)b);
}

int is_sorted(int *array, size_t n) {
    if (n <= 1) return 1;
    for (size_t i = 0; i < n-1; i++) {
        if (array[i] > array[i+1]) return 0;
    }
    return 1;
}

void generate_array(int *array, size_t n, int type) {
    switch (type) {
        case 0: 
            for (size_t i = 0; i < n; i++) array[i] = rand() % n;
            break;
        case 1: 
            for (size_t i = 0; i < n; i++) array[i] = i;
            break;
        case 2: 
            for (size_t i = 0; i < n; i++) array[i] = n - i;
            break;
        case 3: 
            for (size_t i = 0; i < n/2; i++) array[i] = i;
            for (size_t i = n/2; i < n; i++) array[i] = n - i;
            break;
        case 4: 
            for (size_t i = 0; i < n/2; i++) {
                array[2*i] = i;
                array[2*i+1] = n - i;
            }
            if (n % 2) array[n-1] = n/2;
            break;
    }
}

void test_correctness() {
    int tests[][20] = {{0}, {1}, {2,1}, {1,2}, {5,5,5}, 
                      {1,2,3,4,5}, {5,4,3,2,1}, {3,1,4,2,5}};
    int sizes[] = {0,1,2,2,3,5,5,5};

    for (int i = 0; i < 8; i++) {
        qsort(tests[i], sizes[i], sizeof(int), int_compare);
        printf("Test %d: %s\n", i+1, 
               is_sorted(tests[i], sizes[i]) ? "PASSED" : "FAILED");
    }
}

void test_performance(int type) {
    size_t n = 10000000;
    int *array = malloc(n * sizeof(int));
    generate_array(array, n, type);

    clock_t start = clock();
    qsort(array, n, sizeof(int), int_compare);
    clock_t end = clock();

    double time = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("Type %d: %.3f seconds\n", type, time);
    free(array);
}

int main() {
    srand(time(NULL));
    printf("=== Correctness Tests ===\n");
    test_correctness();

    printf("\n=== Performance (n=10,000,000) ===\n");
    for (int i = 0; i <= 4; i++) {
        test_performance(i);
    }
    return 0;
}
