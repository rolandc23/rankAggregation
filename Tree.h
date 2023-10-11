// Interface to the Tree ADT
// Adapted from assignment 1

#ifndef TREE_H
#define TREE_H

#include <stdio.h>
#include <stdbool.h>
#include "List.h"
#include "Set.h"
#include "Record.h"

typedef struct tree *Tree;
typedef struct tnode *tNode;

struct tree {
    tNode root;
    int (*compare)(Record, Record);

    // IMPORTANT: Do not modify the fields above
    // You may add additional fields below if necessary

};

struct tnode {
    Record rec;
    tNode   left;
    tNode   right;

    int height;
    int balance;

};

////////////////////////////////////////////////////////////////////////
// Provided functions

/**
 * Creates a new tree that will use the given comparison function
 */
Tree TreeNew(int (*compare)(Record, Record));

/**
 * Frees the given tree. If freeRecords is true, also frees the records.
 */
void TreeFree(Tree t, bool freeRecords);

////////////////////////////////////////////////////////////////////////
// Functions you need to implement

/**
 * Inserts a record into the tree.
 * Returns true if the record was inserted into the tree, and false if
 * the tree already contains a record that compares equal to the given
 * record.
 * The time complexity of this function must be O(log n).
 */
bool TreeInsert(Tree t, Record rec);

/**
 * Searches the tree for a record that compares equal to the given
 * record. Returns the record if it exists, and NULL otherwise.
 * The time complexity of this function must be O(log n).
 */
Record TreeSearch(Tree t, Record rec);

////////////////////////////////////////////////////////////////////////

#endif

