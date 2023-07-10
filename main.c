#include "header.h"

int main(){
    // Define a character array to store the file name
    char str[10] = "view.txt";

    // Initialize variables for the query rectangle
    int x_high = 0;
    int y_high = 0;
    int x_low = 0;
    int y_low = 0;

    // Declare a pointer to a quadtree structure
    TREE t;
    // Read data from the file and store it in the quadtree
    t = readData(str);

    // Traverse the quadtree in pre-order and print the points
    pre_order_traversal(t->root);
    printf("\n");

    // Define a rectangle structure to represent the query region
    rect r = {.x_high = x_high, .x_low = x_low, .y_low = y_low, .y_high = y_high};

    // Search for points inside the query rectangle and print them
    printf("The following points lie inside/on the query rectangle - \n");
    Search(t->root, r);

    // Return 0 to indicate successful execution of the program
    return 0;
}
