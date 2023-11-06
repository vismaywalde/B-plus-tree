


// vismay walde (2022mcb1283)
// tejas wagh (2022csb1144)
// shivam zample(2022mcb1280)


#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>




/*####################################################################################################################################################
                              B tree code   
####################################################################################################################################################*/

int t=3; // Minimum degree of the B-Tree

struct btNode   // Structure of the node of B-Tree data structure
{
    int leaf;
    int n;
    int *key;
    struct btNode **c; // Pointers to the children if not leaf
};

typedef struct btNode bTree; // Renaming structure for simplicity
bTree *root = NULL;


// Function prototypes for B_TREE_MERGE_CHILD and B_TREE_REBALANCE_CHILD
void B_TREE_MERGE_CHILD(bTree **node, int i);
void B_TREE_REBALANCE_CHILD(bTree **node, int i);


// Function to allocate memory and create a new node
bTree *B_TREE_CREATE()
{
    bTree *b;
    b = (bTree *)malloc(sizeof(bTree));
    b->leaf = 1;
    b->n = 0;
    b->key = (int *)malloc((2 * t - 1) * sizeof(int));
    b->c = (bTree **)malloc((2 * t) * sizeof(bTree));
    return b;
}


void B_TREE_INORDER_TRAVERSAL(bTree *node) // Function to print Inorder Traversal of the B-Tree.
{
    int j = 0;
    if (node->leaf)
    {
        for (int i = 0; i < node->n; i++)
        {
            printf("%d ", *(node->key + i));
        }
    }
    else
    {

        B_TREE_INORDER_TRAVERSAL(*(node->c + j));
        j++;
        for (int j = 1; j <= node->n; j++)
        {
            printf("%d ", *(node->key + j - 1));
            B_TREE_INORDER_TRAVERSAL(*(node->c + j));
        }
     }
}







// Function to find the minimum element in the tree
void B_TREE_MIN(bTree *node)
{
    if (root == NULL)
    {
        printf("Empty B-Tree\n");
        return;
    }

    if (root->n == 0)
    {
        printf("Empty B-Tree\n");
        return;
    }

    if (!node->leaf)
    {
        B_TREE_MIN(node->c[0]);
    }

    else
    {
        printf("Minimum key in B tree : %d\n", node->key[0]);
    }
}


// Function to search an element in the B-Tree
void B_TREE_SEARCH(bTree *node, int k)
{   int flag=0;
    

    if (root == NULL)
    {
        printf("Empty B-Tree\n");
        return;
    }

    if (root->n == 0)
    {
        printf("Empty B-Tree\n");
        return;
    }
    int i = 0;

    while (i < node->n && k > node->key[i])
    {
        i++;
    }

    if (i < node->n && k == node->key[i])
    { flag =1;
    }
    if(flag == 1){
         printf("Key %d found in Btree.\n", k);
         

    }

    else if (node->leaf )
    {
        printf("Key %d not found in Btree.\n", k);
    }

    else
    {
        B_TREE_SEARCH(node->c[i], k);
    }
}


// Function to split the child into two and pushing the value from child to the above array of keys
void B_TREE_SPLIT_CHILD(bTree *x, int i)
{
    bTree *z = B_TREE_CREATE();
    bTree *y = x->c[i - 1];
    z->leaf = y->leaf;
    z->n = t - 1;
    for (int j = 0; j < t - 1; j++)
    {
        z->key[j] = y->key[j + t];
    }
    if (!y->leaf)
    {
        for (int j = 0; j < t; j++)
        {
            z->c[j] = y->c[j + t];
        }
    }
    y->n = t - 1;
    for (int j = x->n; j >= i; j--)
    {
        x->c[j + 1] = x->c[j];
    }
    x->c[i] = z;
    for (int j = x->n - 1; j >= i - 1; j--)
    {
        x->key[j + 1] = x->key[j];
    }
    x->key[i - 1] = y->key[t - 1];
    x->n++;
}


// Function to insert key if the node is not full
void B_TREE_INSERT_NONFULL(bTree *x, int k)
{
    int i = x->n - 1;
    if (x->leaf)
    {
        while (i >= 0 && k < x->key[i])
        {
            x->key[i + 1] = x->key[i];
            i--;
        }
        x->key[i + 1] = k;
        x->n++;
    }
    else
    {
        while (i >= 0 && k < x->key[i])
        {
            i--;
        }
        i++;
        if (x->c[i]->n == 2 * t - 1)
        {
            B_TREE_SPLIT_CHILD(x, i + 1);
            if (k > x->key[i])
            {
                i++;
            }
        }
        B_TREE_INSERT_NONFULL(x->c[i], k); // Insert k into the child of ith key
    }
}


// Function to insert key in the B-Tree first this function is called then from this above function can be called
void B_TREE_INSERT(bTree **node, int k)
{
    
    bTree *r = root;
    if (r->n == 2 * t - 1)
    {
        bTree *s = B_TREE_CREATE();
        root = s;
        s->leaf = 0;
        s->n = 0;
        s->c[0] = r;
        B_TREE_SPLIT_CHILD(s, 1);
        B_TREE_INSERT_NONFULL(s->c[0], k);
    }
    else
    {
        B_TREE_INSERT_NONFULL(r, k);
    }

 
    
}


// Function to find the predecessor of a key in the B-Tree
int B_TREE_FIND_PREDECESSOR(bTree *node, int k)
{
    if (node == NULL)
    {
        return -1; // Not found
    }

    int i = 0;
    while (i < node->n && k > node->key[i])
    {
        i++;
    }

    if (i < node->n && k == node->key[i])
    {
        // Key found, return it
        return k;
    }

    if (node->leaf)
    {
        if (i == 0)
        {
            // If we are at the leftmost leaf, there is no predecessor
            return -1;
        }
        else
        {
            // Return the key to the left of where the key would be inserted
            return node->key[i - 1];
        }
    }

    // Recursively search in the appropriate child
    int result = B_TREE_FIND_PREDECESSOR(node->c[i], k);

    if (result == -1)
    {
        // If the predecessor was not found in the child, check the keys in the current node
        if (i == 0)
        {
            // If we are at the leftmost child, there is no predecessor
            return -1;
        }
        else
        {
            // Return the key to the left of where the key would be inserted
            return node->key[i - 1];
        }
    }

    return result;
}


// Function to find the successor of a key in the B-Tree
int B_TREE_FIND_SUCCESSOR(bTree *node, int k)
{
    if (node == NULL)
    {
        return -1; // Not found
    }

    int i = 0;
    while (i < node->n && k >= node->key[i])
    {
        i++;
    }

    if (i < node->n && k == node->key[i])
    {
        // Key found, return it
        return k;
    }

    if (node->leaf)
    {
        if (i == node->n)
        {
            // If we are at the rightmost leaf, there is no successor
            return -1;
        }
        else
        {
            // Return the key to the right of where the key would be inserted
            return node->key[i];
        }
    }

    // Recursively search in the appropriate child
    int result = B_TREE_FIND_SUCCESSOR(node->c[i], k);

    if (result == -1)
    {
        // If the successor was not found in the child, check the keys in the current node
        if (i == node->n)
        {
            // If we are at the rightmost child, there is no successor
            return -1;
        }
        else
        {
            // Return the key to the right of where the key would be inserted
            return node->key[i];
        }
    }

    return result;
}


// Function to delete a key from the B-Tree
void B_TREE_DELETE(bTree **node, int k)
{
    if (*node == NULL)
    {
        return; // Key not found
    }

    bTree *rootNode = *node;

    int i = 0;
    while (i < rootNode->n && k > rootNode->key[i])
    {
        i++;
    }

    if (i < rootNode->n && k == rootNode->key[i])
    {
        // Key found in the current node
        if (rootNode->leaf)
        {
            // Case 1: If the key is in a leaf node
            for (int j = i; j < rootNode->n - 1; j++)
            {
                rootNode->key[j] = rootNode->key[j + 1];
            }
            rootNode->n--;
        }
        else
        {
            // Case 2: If the key is in an internal node
            if (rootNode->c[i]->n >= t)
            {
                // Case 2a: If the child containing the predecessor has at least t keys
                int pred = B_TREE_FIND_PREDECESSOR(rootNode->c[i], k);
                B_TREE_DELETE(&rootNode->c[i], pred);
                rootNode->key[i] = pred;
            }
            else if (rootNode->c[i + 1]->n >= t)
            {
                // Case 2b: If the child containing the successor has at least t keys
                int succ = B_TREE_FIND_SUCCESSOR(rootNode->c[i + 1], k);
                B_TREE_DELETE(&rootNode->c[i + 1], succ);
                rootNode->key[i] = succ;
            }
            else
            {
                // Case 2c: If both the child and its successor have t-1 keys
                // Merge the child with its right sibling
                B_TREE_MERGE_CHILD(node, i);
                // Delete k from the merged node
                B_TREE_DELETE(node, k);
            }
        }
    }
    else
    {
        // Key not found in the current node
        if (rootNode->leaf)
        {
            // Key not found in the tree
            return;
        }

        // Determine whether the child where k may exist is present in memory
        int flag = (i == rootNode->n) ? 1 : 0;
        if (rootNode->c[i]->n < t)
        {
            // Case 3a: If the child has less than t keys
            B_TREE_REBALANCE_CHILD(node, i);
            if (flag && i > rootNode->n)
            {
                i--;
            }
        }
        B_TREE_DELETE(&(rootNode->c[i]), k);
    }
}


// Function to merge the i-th child of a node with its right sibling
void B_TREE_MERGE_CHILD(bTree **node, int i)
{
    bTree *leftChild = (*node)->c[i];
    bTree *rightChild = (*node)->c[i + 1];

    // Move the key from the current node to the left child
    leftChild->key[leftChild->n] = (*node)->key[i];
    leftChild->n++;

    // Copy the keys from the right child to the left child
    for (int j = 0; j < rightChild->n; j++)
    {
        leftChild->key[leftChild->n] = rightChild->key[j];
        leftChild->n++;
    }

    // If the children are not leaf nodes, copy the children pointers as well
    if (!leftChild->leaf)
    {
        for (int j = 0; j <= rightChild->n; j++)
        {
            leftChild->c[leftChild->n] = rightChild->c[j];
            leftChild->n++;
        }
    }

    // Remove the key from the current node
    for (int j = i; j < (*node)->n - 1; j++)
    {
        (*node)->key[j] = (*node)->key[j + 1];
    }
    (*node)->n--;

    // Remove the pointer to the right child
    for (int j = i + 1; j < (*node)->n; j++)
    {
        (*node)->c[j] = (*node)->c[j + 1];
    }
    (*node)->c[(*node)->n] = NULL;

    // Free the memory occupied by the right child
    free(rightChild);

    // Check if the current node is empty and needs to be merged with its parent
    if ((*node)->n == 0)
    {
        bTree *temp = *node;
        *node = leftChild;
        free(temp);
    }
}


// Function to rebalance the i-th child of a node
void B_TREE_REBALANCE_CHILD(bTree **node, int i)
{
    if (i > 0 && (*node)->c[i - 1]->n >= t)
    {
        // Case 3b1: Borrow a key from the left sibling
        bTree *child = (*node)->c[i];
        bTree *leftSibling = (*node)->c[i - 1];

        // Make space for the new key at the beginning of the child's keys
        for (int j = child->n; j > 0; j--)
        {
            child->key[j] = child->key[j - 1];
        }

        // Copy the key from the left sibling to the child
        child->key[0] = (*node)->key[i - 1];
        child->n++;

        // Update the parent's key with the left sibling's last key
        (*node)->key[i - 1] = leftSibling->key[leftSibling->n - 1];

        // If the child is not a leaf, move the appropriate child pointer as well
        if (!child->leaf)
        {
            for (int j = child->n; j > 0; j--)
            {
                child->c[j] = child->c[j - 1];
            }
            child->c[0] = leftSibling->c[leftSibling->n];
        }

        // Decrease the left sibling's key count
        leftSibling->n--;

        return;
    }

    if (i < (*node)->n && (*node)->c[i + 1]->n >= t)
    {
        // Case 3b2: Borrow a key from the right sibling
        bTree *child = (*node)->c[i];
        bTree *rightSibling = (*node)->c[i + 1];

        // Copy the key from the right sibling to the child
        child->key[child->n] = (*node)->key[i];
        child->n++;

        // Update the parent's key with the right sibling's first key
        (*node)->key[i] = rightSibling->key[0];

        // If the child is not a leaf, move the appropriate child pointer as well
        if (!child->leaf)
        {
            child->c[child->n] = rightSibling->c[0];
        }

        // Shift the keys in the right sibling
        for (int j = 0; j < rightSibling->n - 1; j++)
        {
            rightSibling->key[j] = rightSibling->key[j + 1];
        }

        // If the right sibling is not a leaf, move the child pointers as well
        if (!rightSibling->leaf)
        {
            for (int j = 0; j < rightSibling->n; j++)
            {
                rightSibling->c[j] = rightSibling->c[j + 1];
            }
        }

        // Decrease the right sibling's key count
        rightSibling->n--;

        return;
    }

    if (i > 0)
    {
        // Case 3b3: Merge with the left sibling
        B_TREE_MERGE_CHILD(node, i - 1);
    }
    else
    {
        // Case 3b4: Merge with the right sibling
        B_TREE_MERGE_CHILD(node, i);
    }
}

int main() {
    
     root = B_TREE_CREATE();   // create Btree
    int choice;
    int key;

    while (1) {
        printf("\nB Tree Operations:\n");
        printf("1. Insert Key\n");
        printf("2. Traversal\n");
       
        printf("3. Search Key\n");
        printf("4. Delete Key\n");
        printf("5. Find Min\n");
        printf("6. Quit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        int minKey;
        

        switch (choice) {
            case 1:
                printf("Enter key to insert: ");
                scanf("%d", &key);
               
                B_TREE_INSERT(&root, key);      //insert in B tree
                break;
            case 2:
                
                
                if (root == NULL)
                {
                    printf("Empty B-Tree\n");
                }
                else
                {   
                    printf("Traversal of Btree: ");
                    B_TREE_INORDER_TRAVERSAL(root);
                }
                
                break;
            
            
               
         
         
         
            case 3:
                printf("Enter key to search: ");
                scanf("%d", &key);   
                clock_t start, end;
                double cpu_time_used;
    
                start = clock(); // Start the clock
                B_TREE_SEARCH(root, key);
                 end = clock(); // End the clock
         cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
         
 printf("Time taken for searching in Btree: %f seconds\n", cpu_time_used);
                 
                break;
            
            case 4:
                printf("Enter key to delete: ");
                scanf("%d", &key);
                B_TREE_DELETE(&root, key);
                break;
            
            case 6:
                exit(0);
            
            case 5:
                // Find the minimum element
                
                B_TREE_MIN(root);
                
                break;
               
            default:
                printf("Invalid choice. Type between (1 to 6 ).\n");
        }
   }

return 0;
}
