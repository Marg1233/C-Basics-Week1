#include <stdio.h>

int main() {
    // Variable declarations
    float length, width, area;

    // Input values
    printf("Enter length of rectangle: ");
    scanf("%f", &length);
    printf("Enter width of rectangle: ");
    scanf("%f", &width);

    // Calculate area
    area = length * width;

    // Output result
    printf("Area of rectangle = %.2f\n", area);

    return 0;
}
