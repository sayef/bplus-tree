# B+ Tree Implementation in C++

This code implements a B+ tree, which is a self-balancing tree data structure that is commonly used in databases to store and retrieve data efficiently. It is similar to a binary search tree, but each node can have more than two children and stores multiple keys.

### `Node` Class

The `Node` class represents a node in the B+ tree. It has several member variables:

*   `keys` is a vector of integers that stores the keys contained in the node.
*   `parent` is a pointer to the parent node of the current node.
*   `children` is a vector of pointers to the children nodes of the current node. This is only used if the current node is not a leaf node.
*   `values` is a vector of integers that stores the values associated with the keys in the node. This is only used if the current node is a leaf node.
*   `next` and `prev` are pointers to the next and previous nodes in the tree, respectively. This is used to create a linked list of leaf nodes to facilitate efficient range queries (not implemented though).
*   `isLeaf` is a boolean that indicates whether the current node is a leaf node or not.

The `Node` class has several member functions:


*   `Node(Node *parent = nullptr, bool isLeaf = false, Node *prev_ = nullptr, Node *next_ = nullptr)` is the constructor for the `Node` class. It takes four optional arguments: a pointer to the parent node, a boolean indicating whether the node is a leaf or not, and pointers to the previous and next nodes in the tree. If the `prev` and `next` arguments are provided, it updates the linked list pointers accordingly.
*   `indexOfChild(int key)` returns the index of the child node that would contain the given key. If the key is not in the node, it returns the index at which the key would be inserted.
*   `indexOfKey(int key)` returns the index of the given key within the `keys` vector. If the key is not found, it returns -1.
*   `getChild(int key)` returns a pointer to the child node that would contain the given key.
*   `setChild(int key, vector<Node*> value)` inserts the given key and vector of child nodes into the `keys` and `children` vectors at the appropriate position.
*   `splitInternal()` is used to split an internal node when it becomes full. It creates a new node and moves half of the keys and children to the new node. It returns a tuple containing the key that will be inserted into the parent node, a pointer to the left node, and a pointer to the right node.
*   `get(int key)` returns the value associated with the given key. If the key is not found, it prints an error message.
*   `set(int key, int value)` inserts the given key and value into the `keys` and `values` vectors at the appropriate position. If the key already exists, it updates the associated value.
*   `splitLeaf()` is used to split a leaf node when it becomes full. It creates a new node and moves half of the keys and values to the new node. It returns a tuple containing the key that will be inserted into the parent node, a pointer to the left node, and a pointer to the right node.
*   `merge(Node* sibling)` is used to merge two leaf nodes when one of them becomes underfull. It moves all of the keys and values from the sibling node into the current node, and updates the linked list pointers accordingly.

### `BPlusTree` class

The `BPlusTree` class represents a B+ tree data structure. It has several member variables:

*   `root`: a pointer to the root node of the tree.
*   `maxCapacity`: an integer indicating the maximum number of keys that a node can have before it needs to be split.
*   `minCapacity`: an integer indicating the minimum number of keys that a node can have before it is considered underfull and may need to borrow a key from a neighboring node or merge with another node.
*   `depth`: an integer indicating the depth of the tree.

The `BPlusTree` class has several member functions:

*   `BPlusTree(int _maxCapacity = 4)` is the constructor for the `BPlusTree` class. It takes an optional argument `_maxCapacity` which specifies the maximum number of keys that a node can have before it needs to be split. If `_maxCapacity` is not provided, the default value of 4 is used. The constructor creates a new root node and initializes the `maxCapacity`, `minCapacity`, and `depth` member variables.
    
*   `Node* findLeaf(int key)` returns a pointer to the leaf node where the given key would be stored. It starts at the root node and traverses down the tree until it reaches a leaf node.
    
*   `int get(int key)` returns the value associated with the given key. It calls the `findLeaf` function to locate the leaf node where the key is stored, and then calls the `get` function of the leaf node to retrieve the value.
    
*   `void set(int key, int value)` inserts the given key and value into the tree. It calls the `findLeaf` function to locate the leaf node where the key would be stored, and then calls the `set` function of the leaf node to insert the key and value. If the leaf node becomes full as a result, it calls the `insert` function to split the node.
    
*   `void insert(tuple<int, Node*, Node*> result)` is used to insert the results of a node split into the tree. It takes a tuple containing the key that will be inserted into the parent node, and pointers to the left and right child nodes. It first checks if the parent node is null, in which case it creates a new root node and updates the `root` and `depth` member variables. Otherwise, it calls the `setChild` function of the parent node to insert the key and child nodes at the appropriate position. If the parent node becomes full as a result, it calls the `insert` function recursively to split the parent node.
    
*   `void remove(int key)` removes the given key from the tree. It first calls the `findLeaf` function to locate the leaf node where the key is stored, and then calls the `removeFromLeaf` function to remove the key from the leaf node. If the leaf node becomes underfull as a result, it checks if the node has a neighboring node with spare keys that it can borrow from, or if it needs to merge with a neighboring node. If the node is the root node and becomes empty as a result of the removal, the root node is updated to point to the only remaining child.
    
*   `void removeFromLeaf(int key, Node *node)` removes the given key from the given leaf node. It removes the key and associated value from the `keys` and `values` vectors, and updates the key in the parent node if necessary.
    
*   `void removeFromInternal(int key, Node *node)` removes the given key from the given internal node. If the key is found in the `keys` vector, it replaces it with the smallest key from the rightmost child. If the internal node becomes underfull as a result, it checks if the node has a neighboring node with spare keys that it can borrow from, or if it needs to merge with a neighboring node.
    
*   `void borrowKeyFromRightLeaf(Node *node, Node *next)` borrows a key from the right neighbor of the given leaf node. It inserts the first key and value from the right neighbor into the given node, and removes the key and value from the right neighbor. It also updates the key in the parent node of the right neighbor if necessary.
    
*   `void borrowKeyFromLeftLeaf(Node *node, Node *prev)` borrows a key from the left neighbor of the given leaf node. It inserts the last key and value from the left neighbor into the given node, and removes the key and value from the left neighbor. It also updates the key in the parent node of the given node if necessary.

*   `void mergeNodeWithRightLeaf(Node *node)` merges the given node with its right neighbor. It appends the keys and values from the right neighbor to the given node, and updates the linked list pointers. It then deletes the right neighbor node.
    
*   `void mergeNodeWithLeftLeaf(Node *node)` merges the given node with its left neighbor. It inserts the keys and values from the left neighbor at the beginning of the `keys` and `values` vectors in the given node, and updates the linked list pointers. It then deletes the left neighbor node.
    
*   `void borrowKeyFromRightInternal(Node *node)` borrows a key from the right neighbor of the given internal node. It inserts the first key and child from the right neighbor into the given node, and removes the key and child from the right neighbor. It also updates the key in the parent node of the right neighbor if necessary.
    
*   `void borrowKeyFromLeftInternal(Node *node)` borrows a key from the left neighbor of the given internal node. It inserts the last key and child from the left neighbor into the given node, and removes the key and child from the left neighbor. It also updates the key in the parent node of the given node if necessary.
    
*   `void mergeNodeWithRightInternal(Node *node)` merges the given internal node with its right neighbor. It appends the keys and children from the right neighbor to the given node, and updates the key in the parent node if necessary. It then deletes the right neighbor node.
    
*   `void mergeNodeWithLeftInternal(Node *node)` merges the given internal node with its left neighbor. It inserts the keys and children from the left neighbor at the beginning of the `keys` and `children` vectors
