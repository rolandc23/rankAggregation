// z5311913
// November 14 2022

// Set.c contains functions which revolve around the
// set data structure
// Each set contains nodes which hold certain information
// about a url, for instance their id, pageRank weight and
// searchScore
// Most functions are utility functions which assist with managing
// sets such as inserting and editing information about urls

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#include <string.h>

#include "Set.h"
#include "pageRank.h"
#include "MergeSort.h"
#include "Record.h"

// Check valid set
bool setValid(Set s) {
    if (s == NULL) {
        return false;
    }
    return true;
}

// Create new set
Set setNew() {
    Set s = malloc(sizeof(*s));
    if (s == NULL) {
        fprintf(stderr, "error: out of memory\n");
        exit(EXIT_FAILURE);
    }
    s->nitems = 0;
    s->head = NULL;
    s->tail = NULL;
    return s;
}

// Free set
void setFree(Set s) {
    Node tmp;
    while (s->head != NULL) {
        tmp = s->head;
        s->head = s->head->next;
        free(tmp->url);
        free(tmp);
    }
    free(s);
}

// Insert at end of the set
void setInsert(Set s, char *url, double weight) {
    assert(setValid(s));
    Node n = nodeNew(s, url, weight);

    if (s->head == NULL) {
        s->head = n;
    } else {
        s->tail->next = n;
    }
    s->tail = n;
    s->nitems++;
}

// Create new node
Node nodeNew(Set s, char *url, double weight) {
    Node n = malloc(sizeof(*n));
    if (n == NULL) {
        fprintf(stderr, "error: out of memory\n");
        exit(EXIT_FAILURE);
    }
    n->url = malloc(sizeof(char*));
    if (n->url == NULL) {
        fprintf(stderr, "error: out of memory\n");
        exit(EXIT_FAILURE);
    }
    n->url = strcpy(n->url, url);
    n->w = weight;
    n->id = s->nitems;
    n->searchScore = 1;
    n->next = NULL;
    return n;
}

// Adds weight to each page-assigned node in the set
void setAddWeight(Set s, int size, double **PR, int lastIt) {
    assert(setValid(s));
    int i = 0;
    for (Node tmp = s->head; tmp != NULL && i < size; tmp = tmp->next, i++) {
        tmp->w = PR[i][lastIt];
    }
}

// Reads in head of node from set to perform mergesort
void setMergeSort(Set s)  {
    MergeSort(&s->head);
}

// Given a set, url and weight, add onto score
// If url does not exist, add new set item to end of list
// Returns the ID
int setAddScore(Set s, char *url, double weight) {
    assert(setValid(s));
    Node tmp = s->head;
    while (tmp != NULL) {
        if (strcmp(url, tmp->url) == 0) {
            tmp->searchScore++;
            return tmp->id;
        }
        tmp = tmp->next;
    }
    setInsert(s, url, weight);
    return s->nitems - 1;
}

// Return size of set
int setSize(Set s) {
    assert(setValid(s));
    return s->nitems;
}