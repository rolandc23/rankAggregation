// COMP2521 Assignment 2
// pageRank.c calculates the pageRank of
// a set of given urls based on their number
// of links going in and out of each page

// Written by: z5311913
// Date: 6/11/2022

#include <ctype.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Graph.h"
#include "Set.h"
#include "List.h"
#include "readData.h"
#include "pageRank.h"
#include "MergeSort.h"

static double numInlinks(Graph g, Vertex v);
static double numOutlinks(List l);

int main(int argc, char *argv[]) {
    // argc is the number of command-line arguments, which includes the
    // program name
    // argv is the array of command-line arguments
    if (argc != 4) {
        fprintf(stderr, "Usage: %s dampingFactor diffPR maxIterations\n",
                argv[0]);
        return EXIT_FAILURE;
    }

    // Command-line arguments are received as strings. They can be converted
    // to number types using atoi and atof.
    double d = atof(argv[1]);
    double diffPR = atof(argv[2]);
    int maxIterations = atoi(argv[3]);

    // Create set of urls from collection.txt
    Set urlList = getCollection();

    // Create graph based on urlList set
    Graph g = getGraph(urlList);
    
    // Create a matrix of doubles storing pagerank value
    // Structure is the following:
    // - PR[urlId][iteration]
    double **PR = createCellMatrix(GraphNumVertices(g), maxIterations);

    // Keep track of total iterations, perform page rank and
    // store information into PR matrix
    int lastIt = pageRank(g, d, diffPR, maxIterations, PR);

    // Add weights of each page into the urlList set
    setAddWeight(urlList, urlList->nitems, PR, lastIt);

    // Sorts urlList set using merge sort
    // Order is from highest to lowest PR
    setMergeSort(urlList);

    Node curr = urlList->head;
    while (curr != NULL)    {

        // Print result on stdout
        printf("%s %0.f %.7f\n", curr->url, outlinkU(g, curr->id), curr->w);

        curr = curr->next;
    }

    // Free graph, PR matrix and urlList set
    GraphFree(g);
    freeCellMatrix(PR);
    setFree(urlList);

    return 0;
}

// Calculates a page rank of a page given iteration, d, page-Id and the PR matrix
// which contains PR values of previous iterations
double calcPageRank(Graph g, Vertex v, double d, int iteration, double **PR) {
    int n = GraphNumVertices(g);
    double newPR = ((1.0 - d) / n) + (d * sumPRwIwO(iteration, g, v, PR));
    return newPR;
}

// Calculates pageRank of every single page and stores onto PR matrix
int pageRank(Graph g, double d, double diffPR, int maxIterations, double **PR) {

    // Get number of vertices or "pages"
    int n = GraphNumVertices(g);

    // Initialise first row of PR values
    for (int i = 0; i < n; i++) {
        PR[i][0] = 1.0 / n;
    }
    
    // Get initial values
    double diff = diffPR;
    int it = 0;
    int lastIt = 0;

    // Loop through until iterations succeeds maxIt or if diff gets
    // greater than diffPR
    while (it < maxIterations && diff >= diffPR) {

        // Initialise sumDiff
        double sumDiff = 0.0;

        // Loop through all pages and calculates PR value of next iteration
        // given page Id and current iteration
        for (int i = 0; i < GraphNumVertices(g); i++) {
            PR[i][it + 1] = calcPageRank(g, i, d, it, PR);
            sumDiff += fabs(PR[i][it + 1] - PR[i][it]);
        }

        // Update iteration, last iteration and diff values
        lastIt = it;
        diff = sumDiff;
        it++;

    }

    // Return last iteration + 1
    return lastIt + 1;
}

// Find sum of the incoming links of outgoing urls
double sumInlinkP(Graph g, Vertex v) {

    // Initialise sum
    double sum = 0.0;

    // Loop through all vertices adjacent to v
    for (adjNode tmp = g->edges[v]->head; tmp != NULL; tmp = tmp->next) {
        sum += numInlinks(g, tmp->v);
    }

    return sum;
}

// Return weightInlink for an edge
double weightInlink(Graph g, Edge e) {
    return numInlinks(g, e.w) / sumInlinkP(g, e.v);
}

// Returns number of outgoing links for a page
double outlinkU(Graph g, Vertex v) {

    // Check if empty list, if so return 0.5 to avoid dividing by 0
    if (ListIsEmpty(g->edges[v])) {
        return 0.5;
    }

    return numOutlinks(g->edges[v]);
}

// Returns number of outgoing links of page's outgoing links
double sumOutlinkP(Graph g, Vertex v) {

    double sum = 0.0;

    for (adjNode tmp = g->edges[v]->head; tmp != NULL; tmp = tmp->next) {
        sum += outlinkU(g, tmp->v);
    }

    return sum;
}

// Return weightOutlink for an edge
double weightOutlink(Graph g, Edge e) {
    return outlinkU(g, e.w) / sumOutlinkP(g, e.v);
}

// Return PR * weightInlink * weightOutlink for a given edge
double PRwIwO(double PR, Graph g, Edge e) {
    return PR * weightInlink(g, e) * weightOutlink(g, e);
}

// Return sum of PR * wI * wO for a page
double sumPRwIwO(int iteration, Graph g, Vertex v, double **PR) {

    // Check for null case
    assert(g != NULL);

    // Create list of incoming links for a page
    List l = ListNew();
    for (int i = 0; i < GraphNumVertices(g); i++) {
        if (g->link[i][v]) {
            ListAppend(l, i);
        }
    }

    // Get sum through looping through list
    double sum = 0.0;
    Edge e;
    for (adjNode n = l->head; n != NULL; n = n->next) {
        e.v = n->v;
        e.w = v;
        sum += PRwIwO(PR[n->v][iteration], g, e);
    }

    // Free list
    ListFree(l);

    return sum;
}

// Helper functions for finding number of inlinks and outlinks for a page

// Get number of inlinks for a given page
static double numInlinks(Graph g, Vertex v) {
    assert(g != NULL);
    int total = 0.0;
    for (int i = 0; i < GraphNumVertices(g); i++) {
        if (g->link[i][v]) {
            total++;
        }
    }
    return total;
}

// Get number of outlinks for a given page
static double numOutlinks(List l) {
    return ListSize(l);
}
