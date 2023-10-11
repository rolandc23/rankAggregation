// readData.c contains functions which are used to read data from files
// and convert them to respective interpretations such as arrays, 
// graphs or sets

#ifndef READDATA_H
#define READDATA_H

#include <stdio.h>
#include "Graph.h"
#include "Set.h"
#include "SearchDb.h"

typedef int Vertex;

// Given a collection.txt file, reads in all urls
Set getCollection(void);

// Converts given set into graph, returning the graph which contains
// adjacency lists and adjacency matrix
Graph getGraph(Set s);

// Given a url file, add outgoing links ("edges") to the graph vertex
void addEdges(Graph g, Set s, Vertex v, FILE *);

// Finds the vertex ID corresponding to a url
// Returns FAIL if failed
Vertex getvID(Set s, char *url);

// Reads in invertedIndex.txt and creates
// a search database which uses AVL tree insertion
SearchDb getInverted();

// Return weight of given page url
// Return 0 if url does not exist
double getPageRank(char *url);


#endif
