#include <stdio.h>
int main() {
// Print my information
printf("My name: margaret\n"); 
printf("My age: 22\n");         
printf("My favorite hobby: cooking\n\n");  
// Get user input
char name[50];
int age;
printf("Enter your name: ");
scanf("%49s", name);
printf("Enter your age: ");
scanf("%d", &age);
// Print greeting message
printf("\nHello %s! You are %d years old. Welcome to the land of music!\n", name, age);
return 0;
}
