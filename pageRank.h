// pageRank file contains functions calculating the pageRank of
// a set of given urls based on their number of links going in 
// and out of each page

#ifndef PAGERANK_H
#define PAGERANK_H

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>

#include "Graph.h"
#include "Set.h"
#include "readData.h"
#include "List.h"

// Calculates a page rank of a page given iteration, d, page-Id and the PR matrix
// which contains PR values of previous iterations
double calcPageRank(Graph g, Vertex v, double d, int iteration, double **PR);

// Calculates pageRank of every single page and stores onto PR matrix
int pageRank(Graph g, double d, double diffPR, int maxIterations, double **PR);

// Find sum of the incoming links of outgoing urls
double sumInlinkP(Graph g, Vertex v);

// Return weightInlink for an edge
double weightInlink (Graph g, Edge e);

// Returns number of outgoing links for a page
double outlinkU(Graph g, Vertex v);

// Returns number of outgoing links of page's outgoing links
double sumOutlinkP(Graph g, Vertex v);

// Return weightOutlink for an edge
double weightOutlink (Graph g, Edge e);

// Return PR * weightInlink * weightOutlink for a given edge
double PRwIwO (double PR, Graph g, Edge e);

// Return sum of PR * wI * wO for a page
double sumPRwIwO (int iteration, Graph g, Vertex v, double **PR); 

#endif