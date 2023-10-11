// Interface to the String List ADT
// Adapted from lab 5 COMP2521

// !!! DO NOT MODIFY THIS FILE !!!

#ifndef LIST_H
#define LIST_H

#include <stdbool.h>

typedef struct list *List;
typedef struct adjNode *adjNode;
typedef int Vertex;

struct adjNode {
    Vertex v;
    adjNode next;
};

struct list {
    adjNode   head;
    adjNode   tail;
    double    size;
};

adjNode newNode(Vertex v);

// Creates a new empty list
// Complexity: O(1)
List ListNew(void);

// Frees all memory allocated for the given list
// Complexity: O(n)
void ListFree(List l);

// Adds a string to the end of the list. Makes a copy of the string
// before adding it to the list.
// Complexity: O(1)
void ListAppend(List l, Vertex v);

// Returns the number of items in the list
// Complexity: O(1)
int  ListSize(List l);

// Checks if a list contains a vertex
bool ListContains(List l, Vertex v);

// Check if list is empty
bool ListIsEmpty(List l);

#endif

