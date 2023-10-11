// Implementation of the String List ADT
// Adapted from lab 5 in COMP2521

// Written by: z5311913
// Date: 10/11/2022


// !!! DO NOT MODIFY THIS FILE !!!

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "List.h"

////////////////////////////////////////////////////////////////////////

// Creates a new empty list
List ListNew(void) {
    List l = malloc(sizeof(*l));
    if (l == NULL) {
        fprintf(stderr, "error: out of memory\n");
        exit(EXIT_FAILURE);
    }
    l->head = NULL;
    l->tail = NULL;
    l->size = 0.0;
    return l;
}

// Frees all memory allocated for the given list
void ListFree(List l) {
    adjNode curr = l->head;
    while (curr != NULL) {
        adjNode temp = curr;
        curr = curr->next;
        free(temp);
    }
    free(l);
}

// Adds a string to the end of the list
void ListAppend(List l, Vertex v) {
    adjNode n = newNode(v);
    if (l->head == NULL) {
        l->head = n;
    } else {
        l->tail->next = n;
    }
    l->tail = n;
    l->size++;
}

adjNode newNode(Vertex v) {
    adjNode n = malloc(sizeof(*n));
    if (n == NULL) {
        fprintf(stderr, "error: out of memory\n");
        exit(EXIT_FAILURE);
    }
    n->v = v;
    n->next = NULL;
    return n;
}

// Returns the number of items in the list
int  ListSize(List l) {
    return l->size;
}

// Checks if a list contains a vertex
// Return true if does
// Return false if doesn't
bool ListContains(List l, Vertex v) {
    if (l == NULL || l->head == NULL) {
        return false;
    }
    for (adjNode n = l->head; n != NULL; n = n->next) {
        if (n->v == v) {
            return true;
        }
    }
    return false;
}

// Check if list is empty
bool ListIsEmpty(List l) {
    if (l->head == NULL) {
        return true;
    }
    return false;
}

////////////////////////////////////////////////////////////////////////
