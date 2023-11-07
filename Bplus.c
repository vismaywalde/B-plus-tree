// walde vismay (2022mcb1283)
// tejas wagh (2022csb1144)
// shivam zample (2022mcb1280)

/*##############################################################################################################################################################################
#####################################################################################################################################################################################
                                                                     B+ TREE CODE
####################################################################################################################################################################################
##############################################################################################################################################################################*/

// Include necessary libraries
#include <stdbool.h> // Library for Boolean data type
#include <stdio.h>   // Standard I/O functions
#include <stdlib.h>  // Standard library functions (malloc, free, etc.)
#include <string.h>  // String manipulation functions
#include <time.h>    // Time-related functions

// Default order for B+ Tree
#define ORDER 3 // Define the default order of the B+ Tree

// Node structure for the B+ Tree
typedef struct node
{

  void **Child_Set;    // Child_Set to other nodes or records
  int *keys;           // Keys in the node
  struct node *parent; // Pointer to the parent node
  bool is_leaf;        // Flag indicating if it's a leaf node
  int total_Keys;      // Number of keys currently in the node
  struct node *next;   // Pointer to the next node
} node;

// Function prototypes for related operations in the B+ Tree
node *insert_Into_NewRoot(node *left, int key, node *right);
node *insert_Into_Node(node *root, node *n, int left_index, int key, node *right);
node *insert_Into_Node_After_Splitting(node *root, node *old_node, int left_index, int key, node *right);
node *insert_Into_Leaf(node *leaf, int key);
node *insert_Into_LeafAfterSplitting(node *root, node *leaf, int key);
node *dequeue(void);
int height(node *const root);
int path_To_Leaves(node *const root, node *child);
void print_Leaves(node *const root);
void Tree_printer(node *const root);
void find_And_Print(node *const root, int key, bool flag);
void find_And_PrintRange(node *const root, int range1, int range2, bool flag);
int find_Range(node *const root, int key_start, int key_end, bool flag, int returned_keys[], void *returned_Child_Set[]);
node *findLeaf(node *const root, int key, bool flag);
int cut(int length);
node *insert_Into_Parent(node *root, node *left, int key, node *right);
void enqueue(node *new_node);
void printMenu();
void execute_Operation(node **root);
void insert_Operation(node **root);
void find_And_PrintOperation(node *root);

// Record structure for the B+ Tree
typedef struct record
{
  int value; // Value associated with the record
} record;

// Global variables for B+ Tree initialization
int order = ORDER;    // B+ Tree order (initialized with the default order)
node *queue = NULL;   // Queue used for tree operations
bool Flagger = false; // Flag for flag output during tree operations

// Function to print the keys of the leaf nodes in the B+ tree
void print_Leaves(node *const root)
{
  if (root == NULL)
  {
    // Print "Empty tree" if the tree is empty
    printf("Empty tree.\n");
    return;
  }

  int i;          // Iterator for keys
  node *c = root; // Initialize a temporary node pointer 'c' to the root

  // Traverse down the tree until a leaf node is reached
  while (!c->is_leaf)
    c = c->Child_Set[0];

  while (true)
  {
    // Loop through the keys in the leaf node 'c'
    for (i = 0; i < c->total_Keys; i++)
    {
      // Check if 'Flagger' is true and print the Child_Set
      if (Flagger)
        printf("%p ", c->Child_Set[i]);

      // Print the key values
      printf("%d ", c->keys[i]);
    }

    // Additional pointer if it exists and 'Flagger' is true
    if (Flagger)
      printf("%p ", c->Child_Set[order - 1]);

    // Traverse to the next leaf node or break the loop if at the last leaf node
    if (c->Child_Set[order - 1] != NULL)
    {
      printf(" | ");
      c = c->Child_Set[order - 1];
    }
    else
    {
      break;
    }
  }

  // Print a newline to finalize the display of the leaf nodes
  printf("\n");
}

// Calculate the height of the B+ tree from the root node
int height(node *const root)
{
  int h = 0;      // Initialize height counter
  node *c = root; // Set a temporary node pointer to the root
  while (!c->is_leaf)
  {                      // Traverse down the tree until reaching a leaf node
    c = c->Child_Set[0]; // Move to the first child of the current node
    h++;                 // Increment the height count
  }
  return h; // Return the calculated height of the tree
}

// Get the path length to the leaves from a given node in the B+ tree
int path_To_Leaves(node *const root, node *child)
{
  int length = 0;  // Initialize the path length counter
  node *c = child; // Set a temporary node pointer to the specified child node
  while (c != root)
  {                // Traverse up the tree until reaching the root
    c = c->parent; // Move to the parent of the current node
    length++;      // Increment the path length count
  }
  return length; // Return the path length from the child to the root
}

// Enqueue: Add a node to the end of the queue
void enqueue(node *new_node)
{
  node *c; // Temporary node pointer

  if (queue == NULL)
  {                     // If the queue is empty
    queue = new_node;   // Set the new node as the queue
    queue->next = NULL; // Initialize its next pointer as NULL
  }
  else
  {
    c = queue; // Set c to the beginning of the queue

    // Traverse the queue to find the last node
    while (c->next != NULL)
    {
      c = c->next;
    }

    c->next = new_node;    // Set the next pointer of the last node to the new node
    new_node->next = NULL; // Initialize the next pointer of the new node as NULL (end of the queue)
  }
}

// Dequeue: Remove the node at the beginning of the queue and return it
node *dequeue(void)
{
  node *n = queue;     // Assign the first node in the queue to n
  queue = queue->next; // Move the queue to the next node
  n->next = NULL;      // Disconnect the retrieved node from the queue
  return n;            // Return the node that was dequeued
}

// Print the tree structure starting from the root node
void Tree_printer(node *const root)
{
  node *n = NULL;   // Initialize a node pointer
  int i = 0;        // Initialize a counter variable
  int rank = 0;     // Initialize rank for the tree level
  int new_rank = 0; // Initialize a variable to hold the new rank

  if (root == NULL)
  {                          // If the tree is empty
    printf("Empty tree.\n"); // Display a message indicating an empty tree
    return;                  // Exit the function
  }

  queue = NULL;  // Clear the queue
  enqueue(root); // Add the root node to the queue

  // Traversal loop to display tree nodes in breadth-first order
  while (queue != NULL)
  {
    n = dequeue(); // Retrieve a node from the queue for processing

    // Logic to handle different levels of the tree
    if (n->parent != NULL && n == n->parent->Child_Set[0])
    {
      new_rank = path_To_Leaves(root, n); // Calculate the rank for the current node

      if (new_rank != rank)
      {                  // If the new rank differs from the current rank
        rank = new_rank; // Update the rank
        printf("\n");    // Move to a new line to represent a new level in the tree
      }
    }

    if (Flagger)         // flag output mode check
      printf("(%p)", n); // Print the address of the node

    // Display keys in the current node
    for (i = 0; i < n->total_Keys; i++)
    {
      if (Flagger)
        printf("%p ", n->Child_Set[i]); // Print the address of the pointer
      printf("%d ", n->keys[i]);        // Print the key value
    }

    // Enqueue child nodes (Child_Set) for further traversal
    if (!n->is_leaf)
    {
      for (i = 0; i <= n->total_Keys; i++)
        enqueue(n->Child_Set[i]); // Add child nodes to the queue
    }

    // Display the Child_Set or child nodes of the current node
    if (Flagger)
    {
      if (n->is_leaf)
        printf("%p ", n->Child_Set[order - 1]); // Print the last pointer of a leaf node
      else
        printf("%p ", n->Child_Set[n->total_Keys]); // Print the pointer at the last index
    }

    printf("| "); // Separate nodes by pipe symbol for better visibility
  }
  printf("\n"); // Move to a new line after printing the tree
}

// Find and print key range
void find_And_PrintRange(node *const root, int key_start, int key_end, bool flag)
{
  int i;
  // Calculate the number of elements to be returned
  int array_size = key_end - key_start + 1;

  // Allocate memory for returned keys and Child_Set based on the array size
  int *returned_keys = (int *)malloc(array_size * sizeof(int));
  void **returned_Child_Set = (void **)malloc(array_size * sizeof(void *));

  // Check if memory allocation was successful
  if (returned_keys == NULL || returned_Child_Set == NULL)
  {
    perror("Memory allocation failed"); // Display an error message if allocation fails
    exit(EXIT_FAILURE);                 // Exit the program indicating failure
  }

  // Find the range and return the number of elements found
  int num_found = find_Range(root, key_start, key_end, flag, returned_keys, returned_Child_Set);

  // If no elements found within the range
  if (!num_found)
  {
    printf("None found.\n"); // Display a message indicating no elements found
  }
  else
  {
    // Print the keys and Child_Set

    for (i = 0; i < num_found; i++)
    {
      // Print the key and its corresponding pointer
      printf("Key: %d, Location: %p\n", returned_keys[i], returned_Child_Set[i]);
    }
  }

  // Free allocated memory to avoid memory leaks
  free(returned_keys);
  free(returned_Child_Set);
}

// Find the range of keys and Child_Set
int find_Range(node *const root, int key_start, int key_end, bool flag, int returned_keys[], void *returned_Child_Set[])
{
  int i, num_found;
  num_found = 0; // Initialize the count of found elements

  // Find the leaf node where the key start is located
  node *n = findLeaf(root, key_start, flag);

  if (n == NULL)
    return 0; // If the leaf is not found, return zero

  // Iterate to find the start position of the key in the leaf node
  for (i = 0; i < n->total_Keys && n->keys[i] < key_start; i++)
    ;

  // If the key start position is beyond the keys in the leaf node, return zero
  if (i == n->total_Keys)
    return 0;

  // Traverse the nodes to find keys within the range
  while (n != NULL)
  {
    for (; i < n->total_Keys && n->keys[i] <= key_end; i++)
    {
      // Store the found keys and Child_Set
      returned_keys[num_found] = n->keys[i];
      returned_Child_Set[num_found] = n->Child_Set[i]; // marker
      num_found++;
    }
    // Move to the next leaf node
    n = n->Child_Set[order - 1];
    i = 0;
  }
  return num_found; // Return the count of elements found
}

// Find the leaf node where the key is located
node *findLeaf(node *const root, int key, bool flag)
{
  if (root == NULL)
  {
    if (flag)
      printf("Empty tree.\n");
    return root;
  }
  int i = 0;
  node *c = root;

  // Traverse down the tree to locate the leaf node
  while (!c->is_leaf)
  {
    if (flag)
    {
      // Print the key values while traversing down the tree if flag
      printf("[");
      for (i = 0; i < c->total_Keys - 1; i++)
        printf("%d ", c->keys[i]);
      printf("%d] ", c->keys[i]);
    }
    i = 0;

    // Search within the keys to find the correct position to move to
    while (i < c->total_Keys)
    {
      if (key >= c->keys[i])
        i++;
      else
        break;
    }
    if (flag)
      printf("%d ->\n", i);

    // Move to the appropriate child node based on the key
    c = (node *)c->Child_Set[i];
  }

  if (flag)
  {
    // Print the keys of the leaf node if flag
    printf("Leaf [");
    for (i = 0; i < c->total_Keys - 1; i++)
      printf("%d ", c->keys[i]);
    printf("%d] ->\n", c->keys[i]);
  }

  return c; // Return the leaf node where the key is located
}

// Function to split the value length into equal or slightly larger parts
int cut(int length)
{
  if (length % 2 == 0)
    return length / 2; // If length is even, split it in half
  else
    return length / 2 + 1; // If length is odd, split it with an extra unit to the right side
}

// Function to create a new internal node
node *Create_Node(void)
{
  node *new_node;
  new_node = malloc(sizeof(node)); // Allocate memory for the new node

  if (new_node == NULL)
  {
    perror("Node creation."); // Print error message if memory allocation fails
    exit(EXIT_FAILURE);       // Exit the program with failure status
  }

  new_node->keys = malloc((order - 1) * sizeof(int)); // Allocate memory for keys in the new node

  if (new_node->keys == NULL)
  {
    perror("New node keys array."); // Print error message if memory allocation fails
    exit(EXIT_FAILURE);             // Exit the program with failure status
  }
  new_node->Child_Set = malloc(order * sizeof(void *)); // Allocate memory for Child_Set in the new node
  if (new_node->Child_Set == NULL)
  {
    perror("New node Child_Set array."); // Print error message if memory allocation fails
    exit(EXIT_FAILURE);                  // Exit the program with failure status
  }
  new_node->is_leaf = false; // Set the node as non-leaf
  new_node->total_Keys = 0;  // Initialize the number of keys in the node
  new_node->parent = NULL;   // Set the parent as NULL
  new_node->next = NULL;     // Set the next pointer as NULL
  return new_node;           // Return the created node
}

// Function to create a new leaf node
node *Create_Leaf(void)
{
  node *leaf = Create_Node(); // Create a new node and assign it to leaf
  leaf->is_leaf = true;       // Set the node as a leaf
  return leaf;                // Return the created leaf node
}

// Function to get the index of the left node
int Give_left_index(node *parent, node *left)
{
  int left_index = 0;
  while (left_index <= parent->total_Keys && parent->Child_Set[left_index] != left)
    left_index++;    // Find the index of the left node in the parent node
  return left_index; // Return the index of the left node
}

// Function to insert a key into a leaf node
node *insert_Into_Leaf(node *leaf, int key)
{
  int i, insertion_point;
  insertion_point = 0;

  while (insertion_point < leaf->total_Keys && leaf->keys[insertion_point] < key)
    insertion_point++; // Find the correct insertion point for the key in the leaf node

  for (i = leaf->total_Keys; i > insertion_point; i--)
  {
    leaf->keys[i] = leaf->keys[i - 1];           // Shift the keys to the right to accommodate the new key
    leaf->Child_Set[i] = leaf->Child_Set[i - 1]; // Shift the Child_Set accordingly
  }

  leaf->keys[insertion_point] = key;       // Insert the new key into the leaf node
  leaf->Child_Set[insertion_point] = NULL; // No record pointer needed
  leaf->total_Keys++;                      // Increment the count of keys in the leaf
  return leaf;                             // Return the updated leaf node
}

node *insert_Into_LeafAfterSplitting(node *root, node *leaf, int key)
{
  node *new_leaf; // Declare a new node to store the split leaf node
  int *temp_keys; // Temporary array to hold the keys during the splitting process
  int insertion_index, split, new_key, i, j;

  new_leaf = Create_Leaf(); // Create a new leaf node

  temp_keys = malloc(order * sizeof(int)); // Allocate memory for the temporary keys array
  if (temp_keys == NULL)
  {
    perror("Temporary keys array."); // Print an error message if memory allocation fails
    exit(EXIT_FAILURE);              // Exit the program with a failure status
  }

  insertion_index = 0;
  while (insertion_index < order - 1 && leaf->keys[insertion_index] < key)
    insertion_index++; // Find the index where the new key will be inserted into the leaf node

  // Reorganize the keys to insert the new key in the correct order
  for (i = 0, j = 0; i < leaf->total_Keys; i++, j++)
  {
    if (j == insertion_index)
      j++;
    temp_keys[j] = leaf->keys[i];
  }

  temp_keys[insertion_index] = key; // Insert the new key into the temporary keys array

  leaf->total_Keys = 0; // Reset the number of keys in the original leaf node

  split = cut(order - 1); // Determine the split point for the keys

  // Copy the keys from the temporary array to the original leaf node
  for (i = 0; i < split; i++)
  {
    leaf->keys[i] = temp_keys[i];
    leaf->total_Keys++;
  }

  // Copy the keys from the temporary array to the new leaf node
  for (i = split, j = 0; i < order; i++, j++)
  {
    new_leaf->keys[j] = temp_keys[i];
    new_leaf->total_Keys++;
  }

  free(temp_keys); // Free the temporary keys array

  new_leaf->Child_Set[order - 1] = leaf->Child_Set[order - 1]; // Assign Child_Set to the new leaf and original leaf nodes
  leaf->Child_Set[order - 1] = new_leaf;

  // Set the remaining Child_Set to NULL for both leaf nodes
  for (i = leaf->total_Keys; i < order - 1; i++)
    leaf->Child_Set[i] = NULL;
  for (i = new_leaf->total_Keys; i < order - 1; i++)
    new_leaf->Child_Set[i] = NULL;

  new_leaf->parent = leaf->parent; // Assign the parent to the new leaf node
  new_key = new_leaf->keys[0];     // Update the new key with the first key in the new leaf

  return insert_Into_Parent(root, leaf, new_key, new_leaf); // Insert new key and new leaf into their parent
}

// Insert into a node
node *insert_Into_Node(node *root, node *n, int left_index, int key, node *right)
{
  int i;

  // Shift keys and Child_Set to make room for the new key and pointer
  for (i = n->total_Keys; i > left_index; i--)
  {
    n->Child_Set[i + 1] = n->Child_Set[i];
    n->keys[i] = n->keys[i - 1];
  }

  // Insert the new key and pointer
  n->Child_Set[left_index + 1] = right;
  n->keys[left_index] = key;
  n->total_Keys++;

  return root;
}

// Insert into a new root
node *insert_Into_NewRoot(node *left, int key, node *right)
{
  // Create a new root node and set its keys, Child_Set, and parent
  node *root = Create_Node();
  root->keys[0] = key;
  root->Child_Set[0] = left;
  root->Child_Set[1] = right;
  root->total_Keys++;
  root->parent = NULL;
  left->parent = root;
  right->parent = root;

  return root;
}

// Insert into a node after splitting it
node *insert_Into_Node_After_Splitting(node *root, node *old_node, int left_index, int key, node *right)
{
  int i, j, split, k_prime;
  node *new_node, *child;
  int *temp_keys;
  node **temp_Child_Set;

  // Allocate memory for temporary keys and Child_Set
  temp_Child_Set = malloc((order + 1) * sizeof(node *));
  if (temp_Child_Set == NULL)
  {
    exit(EXIT_FAILURE);
  }
  temp_keys = malloc(order * sizeof(int));
  if (temp_keys == NULL)
  {
    exit(EXIT_FAILURE);
  }

  // Copy Child_Set to temporary arrays, excluding the entry at left_index
  for (i = 0, j = 0; i < old_node->total_Keys + 1; i++, j++)
  {
    if (j == left_index + 1)
      j++;
    temp_Child_Set[j] = old_node->Child_Set[i];
  }

  // Copy keys to temporary arrays, excluding the entry at left_index
  for (i = 0, j = 0; i < old_node->total_Keys; i++, j++)
  {
    if (j == left_index)
      j++;
    temp_keys[j] = old_node->keys[i];
  }

  // Insert the new key and right pointer at left_index in the temporary arrays
  temp_Child_Set[left_index + 1] = right;
  temp_keys[left_index] = key;

  split = cut(order); // Calculate the split point

  // Create a new node and copy half of the keys and Child_Set to the old node
  new_node = Create_Node();
  old_node->total_Keys = 0;
  for (i = 0; i < split - 1; i++)
  {
    old_node->Child_Set[i] = temp_Child_Set[i];
    old_node->keys[i] = temp_keys[i];
    old_node->total_Keys++;
  }

  // Set k_prime to the first key in the new node
  old_node->Child_Set[i] = temp_Child_Set[i];
  k_prime = temp_keys[split - 1];

  // Copy the rest of the keys and Child_Set to the new node
  for (++i, j = 0; i < order; i++, j++)
  {
    new_node->Child_Set[j] = temp_Child_Set[i];
    new_node->keys[j] = temp_keys[i];
    new_node->total_Keys++;
  }
  new_node->Child_Set[j] = temp_Child_Set[i];
  free(temp_Child_Set);
  free(temp_keys);

  // Update the parent of the new node and its children
  new_node->parent = old_node->parent;
  for (i = 0; i <= new_node->total_Keys; i++)
  {
    child = new_node->Child_Set[i];
    child->parent = new_node;
  }

  return insert_Into_Parent(root, old_node, k_prime, new_node); // Insert the new node into the parent
}

// Insert a key and right pointer into the parent node
node *insert_Into_Parent(node *root, node *left, int key, node *right)
{
  int left_index; // Declare an integer variable to store the left index in the parent node
  node *parent;   // Declare a pointer to represent the parent node

  parent = left->parent; // Retrieve the parent node of the left node

  // If the parent is NULL, insert a new root node and the keys
  if (parent == NULL)
  {
    return insert_Into_NewRoot(left, key, right);
  }

  // Find the index in the parent where the left node is located
  left_index = Give_left_index(parent, left);

  // If the parent has space, insert the key and the right pointer into the node
  if (parent->total_Keys < order - 1)
  {
    return insert_Into_Node(root, parent, left_index, key, right);
  }

  // If the parent is full, split the node and insert the key and right pointer
  return insert_Into_Node_After_Splitting(root, parent, left_index, key, right);
}

// Find the node that contains the given key
node *find(node *root, int key, bool flag)
{
  if (root == NULL)
  {
    return NULL; // If the tree is empty, return NULL
  }

  int i = 0;         // Initialize an index variable
  node *leaf = NULL; // Initialize a pointer to a leaf node

  leaf = findLeaf(root, key, flag); // Find the leaf node containing the key

  // Search for the key in the leaf node
  for (i = 0; i < leaf->total_Keys; i++)
  {
    if (leaf->keys[i] == key)
      break; // Break the loop when the key is found
  }

  // If the loop reached the end without finding the key, return NULL
  if (i == leaf->total_Keys)
    return NULL;
  else
    return leaf; // Otherwise, return the leaf node containing the key
}

// Create a new tree with a single key
node *Create_New_Tree(int key)
{
  node *root = Create_Leaf(); // Create a new leaf node to serve as the root of the tree
  root->keys[0] = key;        // Set the key value in the root node

  root->parent = NULL; // Set the parent of the root node as NULL (indicating it's the root)
  root->total_Keys++;  // Increment the count of keys in the root node
  return root;         // Return the created root node
}

// Insert a key into the B+ tree
node *insert(node *root, int key)
{
  // void *record = NULL;  // Not used in this function

  node *leaf = NULL; // Create a pointer for the leaf node

  // If the key exists in the tree, return the root as no change is required
  if (find(root, key, false) != NULL)
    return root;

  // If the tree is empty, start a new tree with the provided key
  if (root == NULL)
    return Create_New_Tree(key);

  // Find the leaf node where the key should be inserted
  leaf = findLeaf(root, key, false);

  // If the leaf has space for the new key, insert it into the leaf and return the root
  if (leaf->total_Keys < order - 1)
  {
    leaf = insert_Into_Leaf(leaf, key);
    return root;
  }

  // If the leaf is full, insert the key into the leaf after splitting it
  return insert_Into_LeafAfterSplitting(root, leaf, key);
}

// Function to find the position for inserting a key in the node
int findPosition(node *n, int key)
{
  int position = 0; // Initialize the position variable

  // Iterate through the node's keys to find the correct position for the new key
  while (position < n->total_Keys && n->keys[position] < key)
    position++; // Move to the next key if it's smaller than the provided key

  return position; // Return the index where the new key should be placed
}

// Function to handle deletion of the root node in the B-tree
node *Deletion_In_Root(node *root)
{
  if (root->total_Keys > 0)
    return root; // The root still contains keys, no underflow situation

  if (!root->is_leaf)
  {
    // If the root is an internal node, reassign the root to its first child
    node *newRoot = root->Child_Set[0];
    newRoot->parent = NULL;

    // Free the memory occupied by the keys, Child_Set, and the original root
    free(root->keys);
    free(root->Child_Set);
    free(root);

    // Display a message indicating the deletion and the new root key
    printf("Root deleted; new root is %d\n", newRoot->keys[0]);
    return newRoot;
  }
  else
  {
    // If the root is a leaf node, simply free its memory
    free(root->keys);
    free(root->Child_Set);
    free(root);

    // Display a message indicating that the tree is now empty
    printf("Root deleted; tree is empty now.\n");
    return NULL; // The tree is empty after deletion of the root
  }
}

// Function to redistribute nodes in case of underflow after deletion
node *Nodes_redistributer(node *root, node *leaf, int index)
{
  node *neighbor = leaf->parent->Child_Set[index - 1];

  // If the leaf node is the first child, assign neighbor to the next sibling
  if (index == 0)
    neighbor = leaf->parent->Child_Set[1];

  // Check if the neighboring node has enough keys to redistribute
  if (neighbor->total_Keys > (ORDER - 1) / 2)
  {

    if (leaf->is_leaf)
    {
      // Shifting keys and Child_Set in the leaf node to create space for redistribution
      for (int i = leaf->total_Keys; i > 0; i--)
      {
        leaf->keys[i] = leaf->keys[i - 1];
        leaf->Child_Set[i] = leaf->Child_Set[i - 1];
      }
      // Update the Child_Set and keys in the leaf node and neighbor
      leaf->Child_Set[0] = neighbor->Child_Set[neighbor->total_Keys];
      leaf->keys[0] = neighbor->keys[neighbor->total_Keys - 1];
      leaf->total_Keys++;
      neighbor->total_Keys--;
      leaf->parent->keys[index - 1] = leaf->keys[0];
    }
    else
    {
      // Shifting keys and Child_Set in the non-leaf node to create space for redistribution
      leaf->Child_Set[leaf->total_Keys + 1] = leaf->Child_Set[leaf->total_Keys];
      for (int i = leaf->total_Keys; i > 0; i--)
      {
        leaf->keys[i] = leaf->keys[i - 1];
        leaf->Child_Set[i] = leaf->Child_Set[i - 1];
      }
      leaf->keys[0] = leaf->parent->keys[index - 1];
      leaf->parent->keys[index - 1] = neighbor->keys[neighbor->total_Keys - 1];
      leaf->Child_Set[0] = neighbor->Child_Set[neighbor->total_Keys];
      neighbor->total_Keys--;
      leaf->total_Keys++;
    }
  }
  return root; // Return the updated root node after redistribution
}

// Function to merge nodes after deletion to address underflow
node *mergeNodes(node *root, node *parent, int index)
{
  node *child = parent->Child_Set[index];
  node *neighbor = parent->Child_Set[index - 1];

  // If the child is the first node, neighbor should be the next sibling
  if (index == 0)
    neighbor = parent->Child_Set[1];

  if (child->is_leaf)
  {
    // Merging keys and Child_Set in the leaf node
    neighbor->Child_Set[neighbor->total_Keys] = child->Child_Set[0];
    for (int i = 0; i < child->total_Keys; i++)
    {
      neighbor->keys[neighbor->total_Keys] = child->keys[i];
      neighbor->Child_Set[neighbor->total_Keys + 1] = child->Child_Set[i + 1];
      neighbor->total_Keys++;
    }
  }
  else
  {
    // Merging keys and Child_Set in the non-leaf node
    neighbor->keys[neighbor->total_Keys] = parent->keys[index - 1];
    neighbor->total_Keys++;

    for (int i = 0, j = neighbor->total_Keys; i < child->total_Keys; i++, j++)
    {
      neighbor->keys[j] = child->keys[i];
      neighbor->Child_Set[j] = child->Child_Set[i];
      neighbor->total_Keys++;
    }
    neighbor->Child_Set[neighbor->total_Keys] = child->Child_Set[child->total_Keys];
  }

  // Adjusting Child_Set and keys in the parent node after the merge
  for (int i = index; i < parent->total_Keys - 1; i++)
  {
    parent->keys[i - 1] = parent->keys[i];
    parent->Child_Set[i] = parent->Child_Set[i + 1];
  }
  parent->total_Keys--;

  free(child); // Free the memory of the merged child node
  return root; // Return the updated root node after merging
}

// Function to delete a key from the B+ tree
node *deleteOperation(node *root, int key)
{
  // Check if the tree is empty
  if (root == NULL)
  {
    printf("Tree is empty.\n");
    return NULL;
  }

  // Find the leaf node where the key is present
  node *leaf = findLeaf(root, key, false);

  // Check if the leaf is not found for the provided key
  if (leaf == NULL)
  {
    printf("Key %d not found.\n", key);
    return root;
  }

  int position;
  // Iterate through the leaf's keys to find the key to be deleted
  for (position = 0; position < leaf->total_Keys; position++)
  {
    if (leaf->keys[position] == key)
    {
      // Deleting the key from the leaf node
      for (int i = position; i < leaf->total_Keys - 1; i++)
      {
        leaf->keys[i] = leaf->keys[i + 1];
        leaf->Child_Set[i] = leaf->Child_Set[i + 1];
      }
      leaf->total_Keys--;

      printf("Key %d deleted successfully.\n", key);

      // Check if the node requires redistribution or merging
      if (leaf->total_Keys < (ORDER - 1) / 2)
      {
        // Check if redistribution is possible
        if (leaf != root)
        {
          // Identify the index of the leaf node
          int index = Give_left_index(leaf->parent, leaf);
          node *neighbor;

          // Identify the neighboring node for possible redistribution or merging
          if (index == 0)
            neighbor = leaf->parent->Child_Set[1];
          else
            neighbor = leaf->parent->Child_Set[index - 1];

          // Check if redistribution is possible with a neighboring node
          if (neighbor->total_Keys > (ORDER - 1) / 2)
          {
            root = Nodes_redistributer(root, leaf, index);
          }
          else
          {
            // Perform merging with the neighboring node
            if (index == 0)
              root = mergeNodes(root, leaf->parent, 1);
            else
              root = mergeNodes(root, leaf->parent, index);
          }
        }
      }

      // If the root becomes empty after deletion, update the root pointer
      if (root->total_Keys == 0)
      {
        free(root->keys);
        free(root->Child_Set);
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
void printMenu()
{
  printf("\nMenu:\n");
  printf("1. Insert a value\n");
  printf("2. Search a value\n");
  printf("3. Print the tree\n");
  printf("4. Delete a value\n");
  printf("5. Exit\n");
  printf("Enter your choice: ");
}

// Function for inserting a value
void insert_Operation(node **root)
{
  int key;
  printf("Enter key to insert: ");
  scanf("%d", &key);
  *root = insert(*root, key);
  printf("Key %d inserted.\n", key);
}

// Find the node and print it
void find_And_Print(node *const root, int key, bool flag)
{
  clock_t start, end;
  double cpu_time_used;

  start = clock(); // Record the start time
  node *leaf = NULL;
  record *r = (record *)find(root, key, flag); // Find the record under the given key
  if (r == NULL)
    printf("Record not found under key %d.\n", key); // Print message if the record is not found
  else
    printf("Key %d present\n", key);                        // Print the found key
  end = clock();                                            // Record the end time
  cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC; // Calculate the time taken for search

  printf("Time taken for searching in B+ tree: %f seconds\n", cpu_time_used); // Print the time taken
}

// Function for finding and printing a value
void find_And_PrintOperation(node *root)
{
  int key;
  printf("Enter the key to find and print: ");
  scanf("%d", &key);
  find_And_Print(root, key, 'a');
}

int main()
{
  node *root = NULL;

  // Menu-driven loop
  while (true)
  {
    int choice;
    int num;

    printMenu();          // Display the menu options
    scanf("%d", &choice); // Get the user's choice
    switch (choice)
    {
    case 1:
      insert_Operation(&root); // Perform the insertion operation
      break;
    case 2:
      find_And_PrintOperation(root); // Perform the search operation
      break;
    case 3:
      Tree_printer(root); // Print the B+ tree
      break;
    case 5:
      exit(0); // Exit the program
    case 4:
      printf("Enter key to delete: ");
      scanf("%d", &num);                 // Get the key to be deleted
      root = deleteOperation(root, num); // Perform the delete operation
      break;
    default:
      printf("Invalid choice! Please enter a valid option.\n"); // Display message for an invalid choice
    }
  }

  return 0;
}
