// COMP2521 Assignment 2

// Written by: z5311913
// Date: 10/11/2022

// Functions which perform merge sort on a
// given set list
// Adapted from pseudocode provided in lectures COMP2521

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "Set.h"

#define CHOOSE_A 0
#define CHOOSE_B 1

static int checkPath(Node a, Node b);

// Splits a list into 2 given it's head pointer
void MergeSplit(Node head, Node *front, Node *back) {

    // Define slow and fast nodes
    Node slow = head; 
    Node fast = head->next;
  
    // For each time slow node traverses once, fast node traverses twice
    while (fast != NULL)    {
        fast = fast->next; 
        if (fast != NULL)   {
            slow = slow->next;
            fast = fast->next;
        } 
    }

    // Slow node will end up before the midpoint in the List, so split it in two 
    // at that point. 
    *front = head; 
    *back = slow->next; 
    slow->next = NULL; 
} 

// Merge 2 split lists
Node SortedMerge(Node a, Node b)    {
    Node result = NULL; 
    
    // NULL cases
    if (a == NULL)  {
        return b; 
    } 
    else if (b == NULL)   {
        return a; 
    }
    
    // Checks for which path to go
    int cmp = checkPath(a, b);

    // Choose the item which has higher score, then weight and add to result
    // Recur to next node
    if (cmp == CHOOSE_A) {
        result = a;
        result->next = SortedMerge(a->next, b);
    } else if (cmp == CHOOSE_B) {
        result = b; 
        result->next = SortedMerge(a, b->next);
    }

    return result; 
} 

// Sorts the List by changing next pointers
void MergeSort(Node *head) { 

    // Initialise head var, a and b which will then store the split lists
    Node tmp = *head; 
    Node a = NULL;
    Node b = NULL;
  
    // Cases where set only contains 0 or 1 elements
    if ((tmp == NULL) || (tmp->next == NULL)) { 
        return; 
    } 

    // Split current list into 2 different lists 
    MergeSplit(tmp, &a, &b);  

    // Recursively sort lists within lists 
    MergeSort(&a); 
    MergeSort(&b); 

    // Merge sorted lists after
    *head = SortedMerge(a, b); 

    return;
}

// Checks for the path which result->next should be
static int checkPath(Node a, Node b) {

    // Compare score
    if (a->searchScore > b->searchScore) {
        return CHOOSE_A;
    } else if (a->searchScore < b->searchScore) {
        return CHOOSE_B;
    }

    // Compare weight
    if (a->w > b->w) {
        return CHOOSE_A;
    } else if (a->w < b->w) {
        return CHOOSE_B;
    }

    // Compare alphabetical order
    int cmp = strcmp(a->url, b->url);
    if (cmp < 0) {
        return CHOOSE_A;
    } else {
        return CHOOSE_B;
    }

}