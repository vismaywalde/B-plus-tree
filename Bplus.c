//walde vismay (2022mcb1283)
//tejas wagh (2022csb1144)
//shivam zample (2022mcb1280)


/*######################################################################################################################################################                     
                                                                            B+ TREE CODE
#############################################################################################################################################################*/



// Include necessary libraries
#include <stdbool.h>    // Library for Boolean data type
#include <stdio.h>      // Standard I/O functions
#include <stdlib.h>     // Standard library functions (malloc, free, etc.)
#include <string.h>     // String manipulation functions
#include <time.h>       // Time-related functions

// Default order for B+ Tree
#define ORDER 3         // Define the default order of the B+ Tree

// Node structure for the B+ Tree
typedef struct node {
  void **pointers;      // Pointers to other nodes or records
  int *keys;            // Keys in the node
  struct node *parent;  // Pointer to the parent node
  bool is_leaf;         // Flag indicating if it's a leaf node
  int num_keys;         // Number of keys currently in the node
  struct node *next;    // Pointer to the next node
} node;

// Function prototypes for related operations in the B+ Tree
node *insertIntoNewRoot(node *left, int key, node *right);
node *insertIntoNode(node *root, node *n, int left_index, int key, node *right);
node *insertIntoNodeAfterSplitting(node *root, node *old_node, int left_index, int key, node *right);
node *insertIntoLeaf(node *leaf, int key);
node *insertIntoLeafAfterSplitting(node *root, node *leaf, int key);
node *dequeue(void);
int height(node *const root);
int pathToLeaves(node *const root, node *child);
void printLeaves(node *const root);
void printTree(node *const root);
void findAndPrint(node *const root, int key, bool verbose);
void findAndPrintRange(node *const root, int range1, int range2, bool verbose);
int findRange(node *const root, int key_start, int key_end, bool verbose, int returned_keys[], void *returned_pointers[]);
node *findLeaf(node *const root, int key, bool verbose);
int cut(int length);
node *insertIntoParent(node *root, node *left, int key, node *right);
void enqueue(node *new_node);
void printMenu();
void executeOperation(node **root);
void insertOperation(node **root);
void findAndPrintOperation(node *root);


// Record structure for the B+ Tree
typedef struct record {
  int value;    // Value associated with the record
} record;

// Global variables for B+ Tree initialization
int order = ORDER;          // B+ Tree order (initialized with the default order)
node *queue = NULL;         // Queue used for tree operations
bool verbose_output = false;  // Flag for verbose output during tree operations

// Function to print the keys of the leaf nodes in the B+ tree
void printLeaves(node *const root) {
  if (root == NULL) {
    // Print "Empty tree" if the tree is empty
    printf("Empty tree.\n");
    return;
  }

  int i;             // Iterator for keys
  node *c = root;    // Initialize a temporary node pointer 'c' to the root

  // Traverse down the tree until a leaf node is reached
  while (!c->is_leaf)
    c = c->pointers[0];

  while (true) {
    // Loop through the keys in the leaf node 'c'
    for (i = 0; i < c->num_keys; i++) {
      // Check if 'verbose_output' is true and print the pointers
      if (verbose_output)
        printf("%p ", c->pointers[i]);

      // Print the key values
      printf("%d ", c->keys[i]);
    }

    // Additional pointer if it exists and 'verbose_output' is true
    if (verbose_output)
      printf("%p ", c->pointers[order - 1]);

    // Traverse to the next leaf node or break the loop if at the last leaf node
    if (c->pointers[order - 1] != NULL) {
      printf(" | ");
      c = c->pointers[order - 1];
    } else {
      break;
    }
  }

  // Print a newline to finalize the display of the leaf nodes
  printf("\n");
}


// Calculate the height of the B+ tree from the root node
int height(node *const root) {
  int h = 0; // Initialize height counter
  node *c = root; // Set a temporary node pointer to the root
  while (!c->is_leaf) { // Traverse down the tree until reaching a leaf node
    c = c->pointers[0]; // Move to the first child of the current node
    h++; // Increment the height count
  }
  return h; // Return the calculated height of the tree
}

// Get the path length to the leaves from a given node in the B+ tree
int pathToLeaves(node *const root, node *child) {
  int length = 0; // Initialize the path length counter
  node *c = child; // Set a temporary node pointer to the specified child node
  while (c != root) { // Traverse up the tree until reaching the root
    c = c->parent; // Move to the parent of the current node
    length++; // Increment the path length count
  }
  return length; // Return the path length from the child to the root
}



// Enqueue: Add a node to the end of the queue
void enqueue(node *new_node) {
  node *c; // Temporary node pointer

  if (queue == NULL) { // If the queue is empty
    queue = new_node; // Set the new node as the queue
    queue->next = NULL; // Initialize its next pointer as NULL
  } else {
    c = queue; // Set c to the beginning of the queue

    // Traverse the queue to find the last node
    while (c->next != NULL) {
      c = c->next;
    }

    c->next = new_node; // Set the next pointer of the last node to the new node
    new_node->next = NULL; // Initialize the next pointer of the new node as NULL (end of the queue)
  }
}

// Dequeue: Remove the node at the beginning of the queue and return it
node *dequeue(void) {
  node *n = queue; // Assign the first node in the queue to n
  queue = queue->next; // Move the queue to the next node
  n->next = NULL; // Disconnect the retrieved node from the queue
  return n; // Return the node that was dequeued
}



// Print the tree structure starting from the root node
void printTree(node *const root) {
  node *n = NULL; // Initialize a node pointer
  int i = 0; // Initialize a counter variable
  int rank = 0; // Initialize rank for the tree level
  int new_rank = 0; // Initialize a variable to hold the new rank

  if (root == NULL) { // If the tree is empty
    printf("Empty tree.\n"); // Display a message indicating an empty tree
    return; // Exit the function
  }

  queue = NULL; // Clear the queue
  enqueue(root); // Add the root node to the queue

  // Traversal loop to display tree nodes in breadth-first order
  while (queue != NULL) {
    n = dequeue(); // Retrieve a node from the queue for processing

    // Logic to handle different levels of the tree
    if (n->parent != NULL && n == n->parent->pointers[0]) {
      new_rank = pathToLeaves(root, n); // Calculate the rank for the current node

      if (new_rank != rank) { // If the new rank differs from the current rank
        rank = new_rank; // Update the rank
        printf("\n"); // Move to a new line to represent a new level in the tree
      }
    }

    if (verbose_output) // Verbose output mode check
      printf("(%p)", n); // Print the address of the node

    // Display keys in the current node
    for (i = 0; i < n->num_keys; i++) {
      if (verbose_output)
        printf("%p ", n->pointers[i]); // Print the address of the pointer
      printf("%d ", n->keys[i]); // Print the key value
    }

    // Enqueue child nodes (pointers) for further traversal
    if (!n->is_leaf) {
      for (i = 0; i <= n->num_keys; i++)
        enqueue(n->pointers[i]); // Add child nodes to the queue
    }

    // Display the pointers or child nodes of the current node
    if (verbose_output) {
      if (n->is_leaf)
        printf("%p ", n->pointers[order - 1]); // Print the last pointer of a leaf node
      else
        printf("%p ", n->pointers[n->num_keys]); // Print the pointer at the last index
    }

    printf("| "); // Separate nodes by pipe symbol for better visibility
  }
  printf("\n"); // Move to a new line after printing the tree
}



// Find and print key range
void findAndPrintRange(node *const root, int key_start, int key_end, bool verbose) {
    int i;
    // Calculate the number of elements to be returned
    int array_size = key_end - key_start + 1;

    // Allocate memory for returned keys and pointers based on the array size
    int *returned_keys = (int *)malloc(array_size * sizeof(int));
    void **returned_pointers = (void **)malloc(array_size * sizeof(void *));

    // Check if memory allocation was successful
    if (returned_keys == NULL || returned_pointers == NULL) {
        perror("Memory allocation failed"); // Display an error message if allocation fails
        exit(EXIT_FAILURE); // Exit the program indicating failure
    }

    // Find the range and return the number of elements found
    int num_found = findRange(root, key_start, key_end, verbose, returned_keys, returned_pointers);

    // If no elements found within the range
    if (!num_found) {
        printf("None found.\n"); // Display a message indicating no elements found
    } else {
        // Print the keys and pointers

        for (i = 0; i < num_found; i++) {
            // Print the key and its corresponding pointer
            printf("Key: %d, Location: %p\n", returned_keys[i], returned_pointers[i]);
        }
    }

    // Free allocated memory to avoid memory leaks
    free(returned_keys);
    free(returned_pointers);
}




// Find the range of keys and pointers
int findRange(node *const root, int key_start, int key_end, bool verbose, int returned_keys[], void *returned_pointers[]) {
  int i, num_found;
  num_found = 0; // Initialize the count of found elements

  // Find the leaf node where the key start is located
  node *n = findLeaf(root, key_start, verbose);

  if (n == NULL)
    return 0; // If the leaf is not found, return zero

  // Iterate to find the start position of the key in the leaf node
  for (i = 0; i < n->num_keys && n->keys[i] < key_start; i++)
    ;

  // If the key start position is beyond the keys in the leaf node, return zero
  if (i == n->num_keys)
    return 0;

  // Traverse the nodes to find keys within the range
  while (n != NULL) {
    for (; i < n->num_keys && n->keys[i] <= key_end; i++) {
      // Store the found keys and pointers
      returned_keys[num_found] = n->keys[i];
      returned_pointers[num_found] = n->pointers[i];
      num_found++;
    }
    // Move to the next leaf node
    n = n->pointers[order - 1];
    i = 0;
  }
  return num_found; // Return the count of elements found
}

// Find the leaf node where the key is located
node *findLeaf(node *const root, int key, bool verbose) {
  if (root == NULL) {
    if (verbose)
      printf("Empty tree.\n");
    return root;
  }
  int i = 0;
  node *c = root;

  // Traverse down the tree to locate the leaf node
  while (!c->is_leaf) {
    if (verbose) {
      // Print the key values while traversing down the tree if verbose
      printf("[");
      for (i = 0; i < c->num_keys - 1; i++)
        printf("%d ", c->keys[i]);
      printf("%d] ", c->keys[i]);
    }
    i = 0;

    // Search within the keys to find the correct position to move to
    while (i < c->num_keys) {
      if (key >= c->keys[i])
        i++;
      else
        break;
    }
    if (verbose)
      printf("%d ->\n", i);

    // Move to the appropriate child node based on the key
    c = (node *)c->pointers[i];
  }

  if (verbose) {
    // Print the keys of the leaf node if verbose
    printf("Leaf [");
    for (i = 0; i < c->num_keys - 1; i++)
      printf("%d ", c->keys[i]);
    printf("%d] ->\n", c->keys[i]);
  }

  return c; // Return the leaf node where the key is located
}


// Function to split the value length into equal or slightly larger parts
int cut(int length) {
  if (length % 2 == 0)
    return length / 2; // If length is even, split it in half
  else
    return length / 2 + 1; // If length is odd, split it with an extra unit to the right side
}

// Function to create a new internal node
node *makeNode(void) {
  node *new_node;
  new_node = malloc(sizeof(node)); // Allocate memory for the new node
  if (new_node == NULL) {
    perror("Node creation."); // Print error message if memory allocation fails
    exit(EXIT_FAILURE); // Exit the program with failure status
  }
  new_node->keys = malloc((order - 1) * sizeof(int)); // Allocate memory for keys in the new node
  if (new_node->keys == NULL) {
    perror("New node keys array."); // Print error message if memory allocation fails
    exit(EXIT_FAILURE); // Exit the program with failure status
  }
  new_node->pointers = malloc(order * sizeof(void *)); // Allocate memory for pointers in the new node
  if (new_node->pointers == NULL) {
    perror("New node pointers array."); // Print error message if memory allocation fails
    exit(EXIT_FAILURE); // Exit the program with failure status
  }
  new_node->is_leaf = false; // Set the node as non-leaf
  new_node->num_keys = 0; // Initialize the number of keys in the node
  new_node->parent = NULL; // Set the parent as NULL
  new_node->next = NULL; // Set the next pointer as NULL
  return new_node; // Return the created node
}

// Function to create a new leaf node
node *makeLeaf(void) {
  node *leaf = makeNode(); // Create a new node and assign it to leaf
  leaf->is_leaf = true; // Set the node as a leaf
  return leaf; // Return the created leaf node
}

// Function to get the index of the left node
int getLeftIndex(node *parent, node *left) {
  int left_index = 0;
  while (left_index <= parent->num_keys && parent->pointers[left_index] != left)
    left_index++; // Find the index of the left node in the parent node
  return left_index; // Return the index of the left node
}

// Function to insert a key into a leaf node
node *insertIntoLeaf(node *leaf, int key) {
  int i, insertion_point;
  insertion_point = 0;

  while (insertion_point < leaf->num_keys && leaf->keys[insertion_point] < key)
    insertion_point++; // Find the correct insertion point for the key in the leaf node

  for (i = leaf->num_keys; i > insertion_point; i--) {
    leaf->keys[i] = leaf->keys[i - 1]; // Shift the keys to the right to accommodate the new key
    leaf->pointers[i] = leaf->pointers[i - 1]; // Shift the pointers accordingly
  }

  leaf->keys[insertion_point] = key; // Insert the new key into the leaf node
  leaf->pointers[insertion_point] = NULL; // No record pointer needed
  leaf->num_keys++; // Increment the count of keys in the leaf
  return leaf; // Return the updated leaf node
}


node *insertIntoLeafAfterSplitting(node *root, node *leaf, int key) {
  node *new_leaf; // Declare a new node to store the split leaf node
  int *temp_keys; // Temporary array to hold the keys during the splitting process
  int insertion_index, split, new_key, i, j;

  new_leaf = makeLeaf(); // Create a new leaf node

  temp_keys = malloc(order * sizeof(int)); // Allocate memory for the temporary keys array
  if (temp_keys == NULL) {
    perror("Temporary keys array."); // Print an error message if memory allocation fails
    exit(EXIT_FAILURE); // Exit the program with a failure status
  }

  insertion_index = 0;
  while (insertion_index < order - 1 && leaf->keys[insertion_index] < key)
    insertion_index++; // Find the index where the new key will be inserted into the leaf node

  // Reorganize the keys to insert the new key in the correct order
  for (i = 0, j = 0; i < leaf->num_keys; i++, j++) {
    if (j == insertion_index)
      j++;
    temp_keys[j] = leaf->keys[i];
  }

  temp_keys[insertion_index] = key; // Insert the new key into the temporary keys array

  leaf->num_keys = 0; // Reset the number of keys in the original leaf node

  split = cut(order - 1); // Determine the split point for the keys

  // Copy the keys from the temporary array to the original leaf node
  for (i = 0; i < split; i++) {
    leaf->keys[i] = temp_keys[i];
    leaf->num_keys++;
  }

  // Copy the keys from the temporary array to the new leaf node
  for (i = split, j = 0; i < order; i++, j++) {
    new_leaf->keys[j] = temp_keys[i];
    new_leaf->num_keys++;
  }

  free(temp_keys); // Free the temporary keys array

  new_leaf->pointers[order - 1] = leaf->pointers[order - 1]; // Assign pointers to the new leaf and original leaf nodes
  leaf->pointers[order - 1] = new_leaf;

  // Set the remaining pointers to NULL for both leaf nodes
  for (i = leaf->num_keys; i < order - 1; i++)
    leaf->pointers[i] = NULL;
  for (i = new_leaf->num_keys; i < order - 1; i++)
    new_leaf->pointers[i] = NULL;

  new_leaf->parent = leaf->parent; // Assign the parent to the new leaf node
  new_key = new_leaf->keys[0]; // Update the new key with the first key in the new leaf

  return insertIntoParent(root, leaf, new_key, new_leaf); // Insert new key and new leaf into their parent
}


// Insert into a node
node *insertIntoNode(node *root, node *n, int left_index, int key, node *right) {
  int i;

  // Shift keys and pointers to make room for the new key and pointer
  for (i = n->num_keys; i > left_index; i--) {
    n->pointers[i + 1] = n->pointers[i];
    n->keys[i] = n->keys[i - 1];
  }

  // Insert the new key and pointer
  n->pointers[left_index + 1] = right;
  n->keys[left_index] = key;
  n->num_keys++;

  return root;
}

// Insert into a new root
node* insertIntoNewRoot(node *left, int key, node *right) {
  // Create a new root node and set its keys, pointers, and parent
  node *root = makeNode();
  root->keys[0] = key;
  root->pointers[0] = left;
  root->pointers[1] = right;
  root->num_keys++;
  root->parent = NULL;
  left->parent = root;
  right->parent = root;

  return root;
}

// Insert into a node after splitting it
node *insertIntoNodeAfterSplitting(node *root, node *old_node, int left_index, int key, node *right) {
  int i, j, split, k_prime;
  node *new_node, *child;
  int *temp_keys;
  node **temp_pointers;

  // Allocate memory for temporary keys and pointers
  temp_pointers = malloc((order + 1) * sizeof(node *));
  if (temp_pointers == NULL) {
    exit(EXIT_FAILURE);
  }
  temp_keys = malloc(order * sizeof(int));
  if (temp_keys == NULL) {
    exit(EXIT_FAILURE);
  }

  // Copy pointers to temporary arrays, excluding the entry at left_index
  for (i = 0, j = 0; i < old_node->num_keys + 1; i++, j++) {
    if (j == left_index + 1)
      j++;
    temp_pointers[j] = old_node->pointers[i];
  }

  // Copy keys to temporary arrays, excluding the entry at left_index
  for (i = 0, j = 0; i < old_node->num_keys; i++, j++) {
    if (j == left_index)
      j++;
    temp_keys[j] = old_node->keys[i];
  }

  // Insert the new key and right pointer at left_index in the temporary arrays
  temp_pointers[left_index + 1] = right;
  temp_keys[left_index] = key;

  split = cut(order); // Calculate the split point

  // Create a new node and copy half of the keys and pointers to the old node
  new_node = makeNode();
  old_node->num_keys = 0;
  for (i = 0; i < split - 1; i++) {
    old_node->pointers[i] = temp_pointers[i];
    old_node->keys[i] = temp_keys[i];
    old_node->num_keys++;
  }

  // Set k_prime to the first key in the new node
  old_node->pointers[i] = temp_pointers[i];
  k_prime = temp_keys[split - 1];

  // Copy the rest of the keys and pointers to the new node
  for (++i, j = 0; i < order; i++, j++) {
    new_node->pointers[j] = temp_pointers[i];
    new_node->keys[j] = temp_keys[i];
    new_node->num_keys++;
  }
  new_node->pointers[j] = temp_pointers[i];
  free(temp_pointers);
  free(temp_keys);

  // Update the parent of the new node and its children
  new_node->parent = old_node->parent;
  for (i = 0; i <= new_node->num_keys; i++) {
    child = new_node->pointers[i];
    child->parent = new_node;
  }

  return insertIntoParent(root, old_node, k_prime, new_node); // Insert the new node into the parent
}


// Insert a key and right pointer into the parent node
node *insertIntoParent(node *root, node *left, int key, node *right) {
  int left_index;  // Declare an integer variable to store the left index in the parent node
  node *parent;    // Declare a pointer to represent the parent node

  parent = left->parent;  // Retrieve the parent node of the left node

  // If the parent is NULL, insert a new root node and the keys
  if (parent == NULL) {
    return insertIntoNewRoot(left, key, right);
  }

  // Find the index in the parent where the left node is located
  left_index = getLeftIndex(parent, left);

  // If the parent has space, insert the key and the right pointer into the node
  if (parent->num_keys < order - 1) {
    return insertIntoNode(root, parent, left_index, key, right);
  }

  // If the parent is full, split the node and insert the key and right pointer
  return insertIntoNodeAfterSplitting(root, parent, left_index, key, right);
}



// Find the node that contains the given key
node *find(node *root, int key, bool verbose) {
    if (root == NULL) {
        return NULL;  // If the tree is empty, return NULL
    }

    int i = 0;          // Initialize an index variable
    node *leaf = NULL;  // Initialize a pointer to a leaf node

    leaf = findLeaf(root, key, verbose);  // Find the leaf node containing the key

    // Search for the key in the leaf node
    for (i = 0; i < leaf->num_keys; i++) {
        if (leaf->keys[i] == key)
            break;  // Break the loop when the key is found
    }

    // If the loop reached the end without finding the key, return NULL
    if (i == leaf->num_keys)
        return NULL;
    else
        return leaf;  // Otherwise, return the leaf node containing the key
}





// Create a new tree with a single key
node *startNewTree(int key) {
  node *root = makeLeaf();   // Create a new leaf node to serve as the root of the tree
  root->keys[0] = key;      // Set the key value in the root node

  root->parent = NULL;      // Set the parent of the root node as NULL (indicating it's the root)
  root->num_keys++;         // Increment the count of keys in the root node
  return root;              // Return the created root node
}


// Insert a key into the B+ tree
node *insert(node *root, int key) {
  // void *record = NULL;  // Not used in this function

  node *leaf = NULL;  // Create a pointer for the leaf node

  // If the key exists in the tree, return the root as no change is required
  if (find(root, key, false) != NULL)
    return root;

  // If the tree is empty, start a new tree with the provided key
  if (root == NULL)
    return startNewTree(key);

  // Find the leaf node where the key should be inserted
  leaf = findLeaf(root, key, false);

  // If the leaf has space for the new key, insert it into the leaf and return the root
  if (leaf->num_keys < order - 1) {
    leaf = insertIntoLeaf(leaf, key);
    return root;
  }

  // If the leaf is full, insert the key into the leaf after splitting it
  return insertIntoLeafAfterSplitting(root, leaf, key);
}



// Function to find the position for inserting a key in the node
int findPosition(node *n, int key) {
    int position = 0;  // Initialize the position variable

    // Iterate through the node's keys to find the correct position for the new key
    while (position < n->num_keys && n->keys[position] < key)
        position++;  // Move to the next key if it's smaller than the provided key

    return position;  // Return the index where the new key should be placed
}

// Function to handle deletion of the root node in the B-tree
node *handleRootDeletion(node *root) {
    if (root->num_keys > 0)
        return root; // The root still contains keys, no underflow situation

    if (!root->is_leaf) {
        // If the root is an internal node, reassign the root to its first child
        node *newRoot = root->pointers[0];
        newRoot->parent = NULL;
        
        // Free the memory occupied by the keys, pointers, and the original root
        free(root->keys);
        free(root->pointers);
        free(root);
        
        // Display a message indicating the deletion and the new root key
        printf("Root deleted; new root is %d\n", newRoot->keys[0]);
        return newRoot;
    } else {
        // If the root is a leaf node, simply free its memory
        free(root->keys);
        free(root->pointers);
        free(root);
        
        // Display a message indicating that the tree is now empty
        printf("Root deleted; tree is empty now.\n");
        return NULL; // The tree is empty after deletion of the root
    }
}



// Function to redistribute nodes in case of underflow after deletion
node *redistributeNodes(node *root, node *leaf, int index) {
    node *neighbor = leaf->parent->pointers[index - 1];

    // If the leaf node is the first child, assign neighbor to the next sibling
    if (index == 0)
        neighbor = leaf->parent->pointers[1];

    // Check if the neighboring node has enough keys to redistribute
    if (neighbor->num_keys > (ORDER - 1) / 2) {

        if (leaf->is_leaf) {
            // Shifting keys and pointers in the leaf node to create space for redistribution
            for (int i = leaf->num_keys; i > 0; i--) {
                leaf->keys[i] = leaf->keys[i - 1];
                leaf->pointers[i] = leaf->pointers[i - 1];
            }
            // Update the pointers and keys in the leaf node and neighbor
            leaf->pointers[0] = neighbor->pointers[neighbor->num_keys];
            leaf->keys[0] = neighbor->keys[neighbor->num_keys - 1];
            leaf->num_keys++;
            neighbor->num_keys--;
            leaf->parent->keys[index - 1] = leaf->keys[0];
        } else {
            // Shifting keys and pointers in the non-leaf node to create space for redistribution
            leaf->pointers[leaf->num_keys + 1] = leaf->pointers[leaf->num_keys];
            for (int i = leaf->num_keys; i > 0; i--) {
                leaf->keys[i] = leaf->keys[i - 1];
                leaf->pointers[i] = leaf->pointers[i - 1];
            }
            leaf->keys[0] = leaf->parent->keys[index - 1];
            leaf->parent->keys[index - 1] = neighbor->keys[neighbor->num_keys - 1];
            leaf->pointers[0] = neighbor->pointers[neighbor->num_keys];
            neighbor->num_keys--;
            leaf->num_keys++;
        }
    }
    return root; // Return the updated root node after redistribution
}

// Function to merge nodes after deletion to address underflow
node *mergeNodes(node *root, node *parent, int index) {
    node *child = parent->pointers[index];
    node *neighbor = parent->pointers[index - 1];

    // If the child is the first node, neighbor should be the next sibling
    if (index == 0)
        neighbor = parent->pointers[1];

    if (child->is_leaf) {
        // Merging keys and pointers in the leaf node
        neighbor->pointers[neighbor->num_keys] = child->pointers[0];
        for (int i = 0; i < child->num_keys; i++) {
            neighbor->keys[neighbor->num_keys] = child->keys[i];
            neighbor->pointers[neighbor->num_keys + 1] = child->pointers[i + 1];
            neighbor->num_keys++;
        }
    } else {
        // Merging keys and pointers in the non-leaf node
        neighbor->keys[neighbor->num_keys] = parent->keys[index - 1];
        neighbor->num_keys++;

        for (int i = 0, j = neighbor->num_keys; i < child->num_keys; i++, j++) {
            neighbor->keys[j] = child->keys[i];
            neighbor->pointers[j] = child->pointers[i];
            neighbor->num_keys++;
        }
        neighbor->pointers[neighbor->num_keys] = child->pointers[child->num_keys];
    }

    // Adjusting pointers and keys in the parent node after the merge
    for (int i = index; i < parent->num_keys - 1; i++) {
        parent->keys[i - 1] = parent->keys[i];
        parent->pointers[i] = parent->pointers[i + 1];
    }
    parent->num_keys--;

    free(child); // Free the memory of the merged child node
    return root; // Return the updated root node after merging
}





// Function to delete a key from the B+ tree
node *deleteOperation(node *root, int key) {
    // Check if the tree is empty
    if (root == NULL) {
        printf("Tree is empty.\n");
        return NULL;
    }

    // Find the leaf node where the key is present
    node *leaf = findLeaf(root, key, false);

    // Check if the leaf is not found for the provided key
    if (leaf == NULL) {
        printf("Key %d not found.\n", key);
        return root;
    }

    int position;
    // Iterate through the leaf's keys to find the key to be deleted
    for (position = 0; position < leaf->num_keys; position++) {
        if (leaf->keys[position] == key) {
            // Deleting the key from the leaf node
            for (int i = position; i < leaf->num_keys - 1; i++) {
                leaf->keys[i] = leaf->keys[i + 1];
                leaf->pointers[i] = leaf->pointers[i + 1];
            }
            leaf->num_keys--;

            printf("Key %d deleted successfully.\n", key);

            // Check if the node requires redistribution or merging
            if (leaf->num_keys < (ORDER - 1) / 2) {
                // Check if redistribution is possible
                if (leaf != root) {
                    // Identify the index of the leaf node
                    int index = getLeftIndex(leaf->parent, leaf);
                    node *neighbor;

                    // Identify the neighboring node for possible redistribution or merging
                    if (index == 0)
                        neighbor = leaf->parent->pointers[1];
                    else
                        neighbor = leaf->parent->pointers[index - 1];

                    // Check if redistribution is possible with a neighboring node
                    if (neighbor->num_keys > (ORDER - 1) / 2) {
                        root = redistributeNodes(root, leaf, index);
                    } else {
                        // Perform merging with the neighboring node
                        if (index == 0)
                            root = mergeNodes(root, leaf->parent, 1);
                        else
                            root = mergeNodes(root, leaf->parent, index);
                    }
                }
            }

            // If the root becomes empty after deletion, update the root pointer
            if (root->num_keys == 0) {
                free(root->keys);
                free(root->pointers);
                free(root);
                printf("Tree is empty now.\n");
                return NULL;
            }

            return root;
        }
    }

    printf("Key %d not found.\n", key);
    return root;
}



// Function to print the menu options
void printMenu() {
    printf("\nMenu:\n");
    printf("1. Insert a value\n");
    printf("2. Search a value\n");
    printf("3. Print the tree\n");
    printf("4. Delete a value\n");
    printf("5. Exit\n");
    printf("Enter your choice: ");
}

// Function to execute the selected operation based on user choice
void executeOperation(node **root) {
    int choice;
    printMenu(); // Display the menu options
    scanf("%d", &choice); // Get the user's choice
    switch (choice) {
        case 1:
            insertOperation(root); // Perform the insertion operation
            break;
        case 2:
            findAndPrintOperation(*root); // Perform the search operation
            break;
        case 3:
            printTree(*root); // Print the B+ tree
            break;
        case 5:
            exit(0); // Exit the program
        case 4:
            int num;
            printf("Enter key to delete: ");
            scanf("%d", &num); // Get the key to be deleted
            *root = deleteOperation(*root, num); // Perform the delete operation
            break;
        default:
            printf("Invalid choice! Please enter a valid option.\n"); // Display message for an invalid choice
    }
}


// Function for inserting a value
void insertOperation(node **root) {
    int key;
    printf("Enter key to insert: ");
    scanf("%d", &key);
    *root = insert(*root, key);
    printf("Key %d inserted.\n", key);
}



// Find the node and print it
void findAndPrint(node *const root, int key, bool verbose) {
    clock_t start, end;
    double cpu_time_used;

    start = clock(); // Record the start time
    node *leaf = NULL;
    record *r = (record *)find(root, key, verbose); // Find the record under the given key
    if (r == NULL)
        printf("Record not found under key %d.\n", key); // Print message if the record is not found
    else
        printf("Key %d present\n", key); // Print the found key
    end = clock(); // Record the end time
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC; // Calculate the time taken for search

    printf("Time taken for searching in B+ tree: %f seconds\n", cpu_time_used); // Print the time taken
}




// Function for finding and printing a value
void findAndPrintOperation(node *root) {
    int key;
    printf("Enter the key to find and print: ");
    scanf("%d", &key);
    findAndPrint(root, key, 'a');
}




int main() {
    node *root = NULL;

    // Menu-driven loop
    while (true) {
        executeOperation(&root);
    }

return 0;
}
