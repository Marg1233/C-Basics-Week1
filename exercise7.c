#include <stdio.h>

int main() {
    int i, n;
    
    // Get the upper limit from user
    printf("Enter n: ");
    scanf("%d", &n);
    
    // Loop from 1 to n
    for (i = 1; i <= n; i++) {
        // Check if number is even
        if (i % 2 == 0) {
            // Print even numbers normally
            printf("%d ", i);
        } else {
            // Print odd numbers in brackets
            printf("[%d] ", i);
        }
    }
    
    // Add new line at the end
    printf("\n");
    
    return 0;
}
