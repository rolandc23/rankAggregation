// Implementation of the Tree ADT
// z5311913 25 October 2022
// Used in assignment 1 COMP2521

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "SearchDb.h"
#include "Record.h"
#include "Tree.h"
#include "readData.h"
#include "MergeSort.h"
#include "Set.h"
#include "List.h"

typedef struct tree *Tree;

static void doTreeFree(tNode n, bool freeRecords);
static tNode doTreeInsertAVL(Tree t, tNode n, Record rec, bool *inserted);
static tNode newTNode(Record rec);
static int getHeight(tNode n);
static tNode checkRotation(Tree t, tNode n, Record rec);
static tNode rotateLeft(tNode n);
static tNode rotateRight(tNode n);
static int max(int a, int b);
static Record doTreeSearch(Tree t, tNode n, Record rec);

////////////////////////////////////////////////////////////////////////
// Provided functions
// !!! DO NOT MODIFY THESE FUNCTIONS !!!

// Create new tree
Tree TreeNew(int (*compare)(Record, Record)) {
    Tree t = malloc(sizeof(*t));
    if (t == NULL) {
        fprintf(stderr, "error: out of memory\n");
        exit(EXIT_FAILURE);
    }

    t->root = NULL;
    t->compare = compare;
    return t;
}

// Free given tree
void TreeFree(Tree t, bool freeRecords) {
    doTreeFree(t->root, freeRecords);
    free(t);
}

// Perform tree freeing done recursively
static void doTreeFree(tNode n, bool freeRecords) {
    if (n != NULL) {
        doTreeFree(n->left, freeRecords);
        doTreeFree(n->right, freeRecords);
        if (freeRecords) {
            RecordFree(n->rec);
        }
        free(n);
    }
}

// Tree Insert //

// Boolean on whether record was inserted or not
bool TreeInsert(Tree t, Record rec) {
    // Default assignment
    bool inserted = false;

    // Insert into tree
    t->root = doTreeInsertAVL(t, t->root, rec, &inserted);
    return inserted;

}

// Inserts record into an AVL tree
static tNode doTreeInsertAVL(Tree t, tNode n, Record rec, bool *inserted) {

    // Null case
    if (n == NULL) {
        *inserted = true;
        return newTNode(rec);
    }

    // Assign compare integer
    int cmp = t->compare(rec, n->rec);

    // Check cases for compare integer
    if (cmp == 0) {
        // Matching record found, no insertion
        return n;

    } else if (cmp < 0) {
        // Insertion occurs on the left
        n->left = doTreeInsertAVL(t, n->left, rec, inserted);

    } else {
        // Insertion occurs on the right
        n->right = doTreeInsertAVL(t, n->right, rec, inserted);
    }
    
    // Assign integers for heights of left and right sub-tree
    int LHeight = getHeight(n->left),
        RHeight = getHeight(n->right);

    // Update height and balance of tNode accordingly
    n->height = 1 + max(LHeight, RHeight);
    n->balance = LHeight - RHeight;

    // Check if rotation is required
    n = checkRotation(t, n, rec);
    
    return n;

}

// Function determining whether height of n
// should be 0 or not.
static int getHeight(tNode n) {
    if (n == NULL) {
        return 0;
    }
    return n->height;
}

// Creates new tNode
static tNode newTNode(Record rec) {

    tNode n = malloc(sizeof(*n));

    // Error if tNode is still null
    if (n == NULL) {
        fprintf(stderr, "error: out of memory\n");
        exit(EXIT_FAILURE);
    }

    // Define left and right pointers and record
    n->left = NULL;
    n->right = NULL;
    n->rec = rec;
    n->height = 1;
    n->balance = 0;

    return n;

}

// Checks if rotation is required to rebalance
// AVL tree
static tNode checkRotation(Tree t, tNode n, Record rec) {

    // Checks for cases where there are imbalances
    if (n->balance > 1) {
        // Left-side imbalance
        if (t->compare(rec, n->left->rec) > 0) {
            // Check if left tNode requires rotation
            n->left = rotateLeft(n->left);
        }
        n = rotateRight(n);

    } else if (n->balance < -1) {
        // Right-side imbalance
        if (t->compare(rec, n->right->rec) < 0) {
            // Check if right tNode requires rotation
            n->right = rotateRight(n->right);
        }
        n = rotateLeft(n);
    }

    return n;

}

// Rotates current tNode of tree to left
static tNode rotateLeft(tNode n) {

    // Null case or right branch is null
    if (n == NULL || n->right == NULL) {
        return n;
    }

    // Left rotation
    tNode m = n->right;
    n->right = m->left;
    m->left = n;

    // Update heights accordingly
    n->height = 1 + max(getHeight(n->left), getHeight(n->right));
    m->height = 1 + max(getHeight(m->left), getHeight(m->right));

    return m;

}

// Rotates the current tNode of tree to the right
static tNode rotateRight(tNode n) {

    // Null case or left branch is null
    if (n == NULL || n->left == NULL) {
        return n;
    }

    // Right rotation
    tNode m = n->left;
    n->left = m->right;
    m->right = n;

    // Update heights accordingly
    n->height = 1 + max(getHeight(n->left), getHeight(n->right));
    m->height = 1 + max(getHeight(m->left), getHeight(m->right));

    return m;
}

// Gets larger of two given integers
static int max(int a, int b) {
    if (a > b) {
        return a;
    } else {
        return b;
    }
}

// Tree Search //

// Performs search of tree for a given record
// Returns record if exists, otherwise return NULL
Record TreeSearch(Tree t, Record rec) {
    return doTreeSearch(t, t->root, rec);
}

// Binary search on a balanced tree for an item
// matching given record
static Record doTreeSearch(Tree t, tNode n, Record rec) {

    // Null case
    if (n == NULL) {
        return NULL;
    }

    // Assign comparison integer
    int cmp = t->compare(rec, n->rec);

    // Check if left or right search is required
    if (cmp < 0) {
        // Left search
        return doTreeSearch(t, n->left, rec);

    } else if (cmp > 0) {
        // Right search
        return doTreeSearch(t, n->right, rec);
        
    } else {
        // Return current record if found
        return n->rec;
    }

}

///////////////////////////////////////////////////////////////
