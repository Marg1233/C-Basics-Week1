#include <stdio.h>

// Function declaration
long long factorial(int n);

int main() {
    int num;
    
    // Get input from user
    printf("Enter a positive integer: ");
    scanf("%d", &num);
    
    // Check for valid input
    if (num < 0) {
        printf("Factorial is not defined for negative numbers.\n");
    } else {
        // Calculate and display factorial
        long long result = factorial(num);
        printf("Factorial of %d = %lld\n", num, result);
    }
    
    return 0;
}

// Function definition to calculate factorial
long long factorial(int n) {
    long long fact = 1;
    
    for (int i = 1; i <= n; i++) {
        fact *= i;
    }
    
    return fact;
}
