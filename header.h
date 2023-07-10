#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<math.h>


#define DIMENSIONS 2
#define m 2
#define M 4
#define CurveOrder 20


typedef struct hilbert_node* NODE;
typedef struct hilbert_r_tree* TREE;


// Define the point struct which stores x , y coordinates and a hilbert value
typedef struct point {
int p_x;
int p_y;
int hilbert_value;
} point;


// Define the rectangle struct which stores the boundaries of a rectangle
typedef struct rect {
int x_low;
int y_high;
int x_high;
int y_low;
} rect;


// Define the structure of each node in the hilbert R-tree
struct hilbert_node {
NODE parent;                // Pointer to the parent node
bool is_leaf;               // Indicates if the node is a leaf or not
point* pnt;                 // Pointer to a point if the node is a leaf
rect rectangle;             // Rectangle defined by the boundaries of the points in the subtree
int number_of_children;     // Number of children nodes
int largest_hilbert_value;  // Largest hilbert value among children in the subtree
NODE* children;             // Array of pointers to child nodes
};


// Define the structure of the hilbert R-tree
struct hilbert_r_tree {
NODE root;                  // Pointer to the root node of the tree
int hilbert_curve_order;    // Order of the hilbert curve used to construct the tree
};


TREE createNewTree();
NODE createNewLeaf(point*);
NODE createNewNode();
void pre_order_traversal(NODE);
void changeFrame(int, int*, int*, int);
int pointToHV (int , int , int );
bool intersect(rect , rect );
void Search(NODE , rect);
void adjust_node(NODE);
NODE chooseLeaf(NODE , point* );
void insertIntoNode(NODE , NODE );
NODE* secondGenList(NODE, int);
int splitCount(int, int );
void balanceTree(NODE, int );
void handleOverflow(NODE, TREE );
TREE adjustTree(NODE,TREE);
TREE insertNewPointinTREE(TREE, point*);
TREE readData(char*);


