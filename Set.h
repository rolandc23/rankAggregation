// Set file contains functions are utility functions which assist with managing
// sets such as inserting and editing information about urls

#ifndef SET_H
#define SET_H

#include <stdio.h>
#include <stdbool.h>

typedef struct node *Node;
typedef struct SetRep *Set;
typedef int Vertex;

// Node storing information about url
struct node {
    char *url;          // url character string
    Vertex id;          // urlId
    double w;           // pageRank weight
    int searchScore;    // score of how many times the url contains search words
    struct node *next;
};

// Set structure
struct SetRep {
    int nitems;         // number of items in set
    Node head;          // pointer to head of set
    Node tail;          // pointer to last node of set
};

// Check valid set
bool setValid(Set s);

// Create new set
Set setNew(void);

// Create new node given a set, url string and weight
Node nodeNew(Set s, char *url, double weight);

// Free set
void setFree(Set s);

// Insert at end of the set
void setInsert(Set s, char *url, double weight);

// Adds weight to each page-assigned node in the set
void setAddWeight(Set s, int size, double **PR, int lastIt);

// Reads in head of node from set to perform mergesort
void setMergeSort(Set s);

// Returns the ID
int setAddScore(Set s, char *url, double weight);

// Return number of items in set
int setSize(Set s);


#endif