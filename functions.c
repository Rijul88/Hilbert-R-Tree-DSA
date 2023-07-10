#include "header.h"


// Create a new Hilbert R-Tree
TREE createNewTree(){
    // Allocate memory for the tree structure
    TREE t = (TREE)malloc(sizeof(struct hilbert_r_tree));
    // Create a new root node for the tree
    t->root = createNewNode();
    // Set the order of the Hilbert curve used by the tree
    t->hilbert_curve_order = CurveOrder;
    return t;
}


NODE createNewLeaf(point* p){
    // Allocate memory for a new Hilbert R-tree node
    NODE n = (NODE)malloc(sizeof(struct hilbert_node));
    // Set the node's flag to indicate it's a leaf
    n->is_leaf = true;
    // Allocate no memory for children to leaf node
    n->children = NULL;
    // Set the largest Hilbert value for the node to the Hilbert value of the given point
    n->largest_hilbert_value = p->hilbert_value;
    // The leaf node has no children
    n->number_of_children = 0;
    // The leaf node has no parent yet
    n->parent = NULL;
    // Set the leaf node's point to the given point
    n->pnt = p;
    // The leaf node's rectangle is a single point
    n->rectangle.x_low = p->p_x;
    n->rectangle.x_high = p->p_x;
    n->rectangle.y_high = p->p_y;
    n->rectangle.y_low = p->p_y;
    // Return the new leaf node
    return n;
}


NODE createNewNode(){
NODE n = (NODE)malloc(sizeof(struct hilbert_node)); // allocate memory for the node
n->is_leaf = false;                             // set the node as an internal node
n->children = (NODE*)calloc(M+1,sizeof(NODE));  // allocate memory for the children of the node
n->largest_hilbert_value = 0;                   // initialize largest Hilbert value to 0
n->number_of_children = 0;                      // initialize number of children to 0
n->parent = NULL;                               // set the parent node as null
n->pnt = NULL;                                  // set the point in the node as null
return n;                                       // return the newly created node
}


void pre_order_traversal(NODE node) {
    // Check if the tree is empty
    if (node == NULL) {
        printf("Tree is empty.");
        return;
    }

    // If node is a leaf, print its point coordinates
    if (node->is_leaf) {
        printf("Leaf node containing point (%d, %d)\n", node->pnt->p_x, node->pnt->p_y);
    } else { 
        // Otherwise, node is an internal node
        // Print the node's minimum bounding rectangle coordinates
        printf("Internal node with MBR: Top-left (%d, %d), Bottom-right (%d, %d)\n",
                node->rectangle.x_low, node->rectangle.y_high, node->rectangle.x_high, node->rectangle.y_low);
        
        // Traverse each child node recursively
        for (int i = 0; i < node->number_of_children; i++) {
            pre_order_traversal(node->children[i]);
        }
    }
}


void changeFrame(int n, int *x, int *y, int quadrant) {
    switch(quadrant) {
        case 3:
            // In quadrant 3, x and y coordinates are mirrored and shifted
            *x = 2*n-1 - *x;
            *y = n-1-*y;
            // No break statement, so it falls through to the next case
        case 0:
            // In quadrant 0, x and y are swapped
            int temp = *x;
            *x = *y;
            *y = temp;
            break;
        case 2:
            // In quadrant 2, x is shifted left by n
            *x -= n;
            // No break statement, so it falls through to the next case
        case 1:
            // In quadrant 1, y is shifted up by n
            *y -= n;
            break;
    }
}


int pointToHV (int x, int y, int n) {
int length = 0; // Initialize length of curve to zero.
int quadrant; // Stores which quadrant the point lies in.
for(int i = pow(2, n-1);i>0;i/=2) {
// Determine which quadrant the point lies in using the binary representation of its coordinates.
quadrant = ((x>=i)&&(y<i))*2+(y>=i)+(x>=i);
// Add the length of the current quadrant to the total length of the curve.
length += i*i*quadrant;
// Rotate and translate the coordinate system according to the quadrant.
changeFrame(i,&x,&y,quadrant);
}
return length; // Return the length of the curve.
}


bool intersect(rect r, rect q) {
   // Check if r is completely to the left of q or vice versa
   if (r.x_high < q.x_low || q.x_high < r.x_low) {
       return false;
   }
   // Check if r is completely above q or vice versa
   if (r.y_low > q.y_high || q.y_low > r.y_high) {
       return false;
   }
   // If the above two conditions are false, then r and q must overlap
   return true;
}


void Search(NODE node, rect R) {
    // Check if tree is empty
    if (node == NULL) {
        printf("Tree is Empty");
        return;
    }
    // If leaf node, check if it lies inside/on the rectangle R
    if (node->is_leaf) {
        if (node->pnt->p_x >= R.x_low && node->pnt->p_x <= R.x_high && 
            node->pnt->p_y <= R.y_high && node->pnt->p_y >= R.y_low) {
            // Print point
            printf("Point ( %d , %d )\n", node->pnt->p_x, node->pnt->p_y);
        }
    } 
    // If internal node, check if it intersects the rectangle R
    else {
        if (intersect(node->rectangle, R)) {
            // Traverse children and search recursively
            for (int i = 0; i < node->number_of_children; i++) {
                Search(node->children[i], R);
            }
        }
    }
}


void adjust_node(NODE node){
   if (node == NULL) {
       return;
   }
   if (node->is_leaf) {
       return;
   }
   if(node->number_of_children==0){
       return;
   }

   // if children are leaf nodes
   if(node->children[0]->is_leaf){
       node->largest_hilbert_value = node->children[0]->pnt->hilbert_value;
       node->rectangle.x_high  =  node->children[0]->pnt->p_x;
       node->rectangle.x_low  =  node->children[0]->pnt->p_x;
       node->rectangle.y_high  =  node->children[0]->pnt->p_y;
       node->rectangle.y_low  =  node->children[0]->pnt->p_y;
       for(int i=0;i<node->number_of_children;i++){
           //update largest_hilbert_value
           if (node->children[i]->pnt->hilbert_value > node->largest_hilbert_value){
               node->largest_hilbert_value = node->children[i]->pnt->hilbert_value;
           }
           //update rectangle
           if( node->children[i]->pnt->p_x > node->rectangle.x_high){
               node->rectangle.x_high  =  node->children[i]->pnt->p_x;
           }
           if( node->children[i]->pnt->p_x < node->rectangle.x_low){
               node->rectangle.x_low  =  node->children[i]->pnt->p_x;
           }
           if( node->children[i]->pnt->p_y > node->rectangle.y_high){
               node->rectangle.y_high  =  node->children[i]->pnt->p_y;
           }
           if( node->children[i]->pnt->p_y < node->rectangle.y_low){
               node->rectangle.y_low  =  node->children[i]->pnt->p_y;
           }
       }
   }
   // if children are not leaf nodes
   else{
       node->largest_hilbert_value = node->children[0]->largest_hilbert_value;
       node->rectangle.x_low  =  node->children[0]->rectangle.x_low;
       node->rectangle.y_high  =  node->children[0]->rectangle.y_high;
       node->rectangle.x_high  =  node->children[0]->rectangle.x_high;
       node->rectangle.y_low  =  node->children[0]->rectangle.y_low;
       for(int i=0;i<node->number_of_children;i++){
           //update largest_hilbert_value
           if (node->children[i]->largest_hilbert_value > node->largest_hilbert_value){
               node->largest_hilbert_value = node->children[i]->largest_hilbert_value;
           }
           //update rectangle
           if( node->children[i]->rectangle.x_low < node->rectangle.x_low){
               node->rectangle.x_low  =  node->children[i]->rectangle.x_low;
           }
           if( node->children[i]->rectangle.y_high > node->rectangle.y_high){
               node->rectangle.y_high  =  node->children[i]->rectangle.y_high;
           }
           if( node->children[i]->rectangle.x_high > node->rectangle.x_high){
               node->rectangle.x_high  =  node->children[i]->rectangle.x_high;
           }
           if( node->children[i]->rectangle.y_low < node->rectangle.y_low){
               node->rectangle.y_low  =  node->children[i]->rectangle.y_low;
           }

       }
   }
}


// Choose a leaf node in the tree where a new point can be inserted
NODE chooseLeaf(NODE node, point* p){
    if(node->children[0] == NULL){ // if the node is a leaf
        return node; // return the leaf node
    }
    else if(node->children[0]->is_leaf){ // if the children of the node are leaf nodes
        return node; // return the node
    }
    else{ // if the children of the node are not leaf nodes
        int i = 0;
        for(; i < node->number_of_children;i++){ // iterate over the children of the node
            if(p->hilbert_value > node->children[i]->largest_hilbert_value){ // if the hilbert value of the point is greater than the largest hilbert value of the child node
                continue; // continue iterating
            }
            else{
                break; // break the loop
            }
        }
        if(i>=node->number_of_children){ // if the loop reached the end of the children of the node
            i = node->number_of_children - 1; // set i to the index of the last child node
        }
        return chooseLeaf(node->children[i],p); // recursively call chooseLeaf on the child node with the closest largest hilbert value
    }
}


void insertIntoNode(NODE parent, NODE child){
child->parent = parent;
int i = 0;
// Find the index at which to insert the new node
for(;i<parent->number_of_children;i++){
    if(child->largest_hilbert_value > parent->children[i]->largest_hilbert_value){
        continue;
    }
    else{
        break;
    }
}
int j = i;
NODE prev = parent->children[i];
// Shift the nodes after the insertion index to make space for the new node
for(;i<parent->number_of_children;i++){
NODE temp = parent->children[i+1];
parent->children[i+1]=prev;
prev = temp;
}
// Insert the new node at the appropriate index
parent->children[j] = child;
parent->number_of_children++;
}


// This function creates a list of all the second generation nodes of a given grandparent node
// The list contains pointers to each of these nodes
// The function takes the grandparent node and the number of children of the grandparent node as input
NODE* secondGenList(NODE grandParent, int noOfChildren)
{
int cnt = 0;
// allocate memory for the list of second generation nodes
NODE* nodelist = (NODE*)malloc((noOfChildren)*sizeof(NODE));
for(int i = 0;i<grandParent->number_of_children;i++)
{
// iterate over the children of the grandparent node
    for(int j =0;j<grandParent->children[i]->number_of_children;j++)
    {
        // add each child of a child node (i.e., a second generation node) to the list
        nodelist[cnt] = grandParent->children[i]->children[j];
        cnt++;
    }
}
// return the list of second generation nodes
return nodelist;
}


int splitCount(int eleCount, int nodeCount)
{
   while (eleCount>=nodeCount)
       eleCount -= nodeCount;
   return eleCount;
  
}


// A function to balance the nodes of the tree by rearranging the children of the parent node
void balanceTree(NODE parent, int sum)
{
    // Create a list of all the second generation children of the parent node
    NODE* nodelist = secondGenList(parent, sum);
    // Reset the number of children and pointers to NULL for each child of the parent node
    int i;
    // Determine the number of children that will go in the first 'splitcnt' children of the parent node
    int counter = 0;
    int splitcnt = splitCount(sum, parent->number_of_children);
    for(i = 0; i<parent->number_of_children; i++)
    {
        for(int j =0; j<parent->children[i]->number_of_children; j++)
        {
            parent->children[i]->children[j] = NULL;
        }
        parent->children[i]->number_of_children = 0;
    }

    // Insert the appropriate children into each child of the parent node
    for(i = 0; i<splitcnt; i++)
    {
        for(int j =0; j<1+(sum/parent->number_of_children); j++)
            insertIntoNode(parent->children[i], nodelist[counter++]);
    }
    for(; i<parent->number_of_children; i++)
    {
        for(int j =0; j<(sum/parent->number_of_children); j++)
            insertIntoNode(parent->children[i], nodelist[counter++]);
    }

    // Free the list of second generation children
    free(nodelist);
}


void handleOverflow(NODE toInsert, TREE rTree)
{
    bool createdNewNode = false;  // flag to indicate whether a new node was created
    int sum = 0;  // sum of the number of children of all nodes at the same level as 'toInsert'
    NODE parent = toInsert->parent;  // parent node of 'toInsert'

    // If 'toInsert' has a parent node
    if(parent!=NULL)
    {
        // Calculate the sum of the number of children of all nodes at the same level as 'toInsert'
        for(int i = 0; i<parent->number_of_children;i++)
        {
            sum+=parent->children[i]->number_of_children;
        }

        // If the sum is less than or equal to M times the number of children of the parent node
        if(sum<=M*parent->number_of_children)
        {
            balanceTree(parent, sum);  // balance the tree
        }
        else  // Otherwise, if the sum is greater than M times the number of children of the parent node
        {
            insertIntoNode(parent, createNewNode());  // insert a new node into the parent node
            balanceTree(parent, sum);  // balance the tree
            createdNewNode = true;  // set the flag to indicate that a new node was created
        }

        // Adjust the nodes at the same level as 'toInsert'
        for(int i = 0; i < parent->number_of_children; i++){
            adjust_node(parent->children[i]);
        }
    }
    else  // If 'toInsert' does not have a parent node
    {
        parent = createNewNode();  // create a new node as the parent of 'toInsert'
        insertIntoNode(parent, toInsert);  // insert 'toInsert' into the parent node
        handleOverflow(toInsert, rTree);  // recursively handle the overflow of the parent node
        if(parent->number_of_children==1)  // If the parent node has only one child
        {
            free(parent);  // free the memory allocated for the parent node
            toInsert->parent = NULL;  // set the parent of 'toInsert' to NULL
        }
        else  // Otherwise, if the parent node has more than one child
        {
            rTree->root = parent;  // set the root of the R-tree to the parent node
        }
    }
}


TREE adjustTree(NODE n, TREE t){
    // Adjust the tree from the leaf node up to the root
    while(n!=NULL){
        adjust_node(n);
        n = n->parent;
    }
    return t;
}


// Insert a new point in the tree
TREE insertNewPointinTREE(TREE t, point* p){
    // Find the leaf node where the point should be inserted
    NODE n = chooseLeaf(t->root,p);
    if(n->number_of_children < 4){
    // Insert the point into the leaf node
    insertIntoNode(n,createNewLeaf(p));
    }else if(n->number_of_children == 4){
        insertIntoNode(n,createNewLeaf(p));
    // Handle overflow if the node has too many children
        while(n->number_of_children == 5){
            handleOverflow(n,t);
            if(n->parent==NULL){
                break;
            }
            n = n->parent;
        }
    }
    // adjust the tree
    t = adjustTree(n,t);
    // Return the updated tree
    return t;
}


TREE readData(char* str12){
    // create a new tree
    TREE t;
    t = createNewTree();
    // open the file with the given filename for reading
    FILE* fp;
    fp = fopen(str12,"r");
    int x, y;
    // read points from the file until the end of file is reached
    while(fscanf(fp, "%d %d", &x, &y) != EOF){
        // create a new point with the x and y coordinates and calculate its hilbert value
        point* p = (point*)malloc(sizeof(point));
        p->p_x = x;
        p->p_y = y;
        p->hilbert_value = pointToHV(x, y, CurveOrder);
        // insert the new point into the tree
        insertNewPointinTREE(t, p);
    }
    // close the file
    fclose(fp);
    // return the new tree
    return t;
}

