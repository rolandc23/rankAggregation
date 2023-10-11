#ifndef MERGESORT_H
#define MERGESORT_H

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#include "Set.h"

// Splits a list into 2 given its head pointer
void MergeSplit(Node n, Node *frontref, Node *backref);

// Merges 2 lists to create one list, returns head pointer
Node SortedMerge(Node a, Node b);

// Performs a merge sort given a list's head
// Splits initial lists into "fragmented lists" and pieces them
// back together in order after
// Time complexity: O(n*log(n)) where n is number of nodes in list
void MergeSort(Node *headRef);

#endif