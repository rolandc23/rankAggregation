// Interface to the Undirected Weighted Graph ADT

#ifndef GRAPH_H
#define GRAPH_H

#include <stdbool.h>
#include "List.h"

typedef struct graph *Graph;

struct graph {
    int nV;         // #vertices
    int nE;         // #edges
    List *edges;    // adjacency list
    bool **link;    // link between 2 edges -> 0 if not directed
                    // i.e. if link[i][j] is true, then j is an outlink of i
                    // and i is an inlink of j
};

typedef int Vertex;

// edges are pairs of vertices (end-points)
typedef struct Edge {
    Vertex v;
    Vertex w;
} Edge;

/**
 * Creates a new instance of a graph
 */
Graph  GraphNew(int nV);

/**
 * Frees all memory associated with the given graph
 */
void GraphFree(Graph g);

/**
 * Returns the number of vertices in the graph
 */
int GraphNumVertices(Graph g);

/**
 * Inserts  an  edge into a graph. Does nothing if there is already an
 * edge between `e.v` and `e.w`. Returns true if successful, and false
 * if there was already an edge.
  */
bool GraphInsertEdge(Graph g, Edge e);

/**
 * Allocates  and  returns  a 2D array/matrix of booleans with the given
 * number of rows and columns. All entries are initialised to false.  It
 * is  the  user's  responsibility  to call `freeBoolMatrix` to free the
 * matrix.
 *
 * Example usage:
 * The following example creates a matrix with 3 rows and 5 columns, and
 * sets the top-left and bottom-right entries to true.
 *
 * bool **matrix = createBoolMatrix(3, 5);
 * matrix[0][0] = true;
 * matrix[2][4] = true;
 *
 * Complexity: O(rc) where r is the number of rows and c is the number
 *             of columns
 */
bool **createBoolMatrix(int nRows, int nCols);

/**
 * Frees the given boolean matrix.
 * Complexity: O(r) where r is the number of rows of the matrix
 */
void freeBoolMatrix(bool **matrix);

/**
 * Allocates and returns a 2D array/matrix of doubles with the given number
 * of rows and columns. All entries are initialised to 0. It is the
 * user's responsibility to call `freeCellMatrix` to free the matrix.
 *
 * Complexity: O(rc) where r is the number of rows and c is the number
 *             of columns
 */
double **createCellMatrix(int nRows, int nCols);

/**
 * Frees the given Cell matrix.
 * Complexity: O(r) where r is the number of rows of the matrix
 */
void freeCellMatrix(double **matrix);

#endif
