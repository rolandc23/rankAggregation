// Implementation of the Directed Weighted Graph ADT
// Uses an adjacency matrix as well as an adjacency list
// Adapted from Lab 7 COMP2521

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "Graph.h"
#include "List.h"

#define INT2VOIDP(i) (void *)(uintptr_t)(i)
#define VOIDP2INT(ptr) (int)(uintptr_t)(ptr)

static int validVertex(Graph g, Vertex v);

////////////////////////////////////////////////////////////////////////

// Creates a new graph
// Stores number of vertices ->nV
// Stores number of edges ->nE
// Stores adjacency lists for each vertex ->edges
// Stores a bool matrix which keeps track of which
// vertices are connected to each other
Graph GraphNew(int nV) {
    assert(nV > 0);

    Graph g = malloc(sizeof(*g));
    if (g == NULL) {
        fprintf(stderr, "error: out of memory\n");
        exit(EXIT_FAILURE);
    }   
    g->nV = nV;
    g->nE = 0;

    g->edges = malloc(nV * sizeof(List));
    g->link = createBoolMatrix(nV, nV);
    if (g->edges == NULL) {
        fprintf(stderr, "error: out of memory\n");
        exit(EXIT_FAILURE);
    }
    
    for (int i = 0; i < nV; i++) {
        g->edges[i] = ListNew();
    }

    return g;
}

// Frees a given graph
void GraphFree(Graph g) {
    for (int i = 0; i < g->nV; i++) {
        ListFree(g->edges[i]);
    }
    free(g->edges);
    freeBoolMatrix(g->link);
    free(g);
}

////////////////////////////////////////////////////////////////////////

// Returns number of vertices given a graph
int GraphNumVertices(Graph g) {
    return g->nV;
}

// Inserts edge into graph given edge
// Returns true if inserted
// Returns false if not inserted
bool GraphInsertEdge(Graph g, Edge e) {

    assert(g != NULL);
    assert(validVertex(g, e.v));
    assert(validVertex(g, e.w));
    assert(e.v != e.w);

    // If link already exists, no need to insert
    if (g->link[e.v][e.w]) {
        return false;
    }

    if (!ListContains(g->edges[e.v], e.w)) {
        ListAppend(g->edges[e.v], e.w);
        g->link[e.v][e.w] = true;
        g->nE++;
        return true;
    }

    return false;
}

// Checks if a vertex is valid
static int validVertex(Graph g, Vertex v) {
    return v >= 0 && v < g->nV;
}

////////////////////////////////////////////////////////////////////////
// Matrix functions from week 7 Lab //

// Creation of bool matrix, initialises all coordinates as false
bool **createBoolMatrix(int nRows, int nCols) {
    bool **m = malloc((nRows + 1) * sizeof(bool *));
    if (m == NULL) {
        fprintf(stderr, "error: out of memory");
        exit(EXIT_FAILURE);
    }
    m[0] = INT2VOIDP(nRows);
    for (int i = 1; i <= nRows; i++) {
        m[i] = calloc(nCols, sizeof(bool));
        if (m[i] == NULL) {
            fprintf(stderr, "error: out of memory");
            exit(EXIT_FAILURE);
        }
    }
    return &m[1];
}

// Frees a given bool matrix
void freeBoolMatrix(bool **matrix) {
    int nRows = VOIDP2INT(matrix[-1]);
    for (int i = 0; i < nRows; i++) {
        free(matrix[i]);
    }
    free(&matrix[-1]);
}

// Creates a matrix of doubles, initialises all coordinates as 0.0
double **createCellMatrix(int nRows, int nCols) {
    double **m = malloc((nRows + 1) * sizeof(double *));
    if (m == NULL) {
        fprintf(stderr, "error: out of memory");
        exit(EXIT_FAILURE);
    }
    m[0] = INT2VOIDP(nRows);
    for (int i = 1; i <= nRows; i++) {
        m[i] = calloc(nCols, sizeof(double));
        if (m[i] == NULL) {
            fprintf(stderr, "error: out of memory");
            exit(EXIT_FAILURE);
        }
    }
    return &m[1];
}

// Frees a given double matrix
void freeCellMatrix(double **matrix) {
    int nRows = VOIDP2INT(matrix[-1]);
    for (int i = 0; i < nRows; i++) {
        free(matrix[i]);
    }
    free(&matrix[-1]);
}