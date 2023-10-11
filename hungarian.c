// COMP2521 Assignment 2

// Written by: z5311913
// Date: 10/11/2022

#include <assert.h>
#include <ctype.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "Set.h"
#include "Graph.h"

#define MAX_NUM_FILES 1000
#define MAX_NUM_URL 100
#define MAX_URL_LENGTH 1000
#define DEF_LOWEST 100.0
#define MAX_COORD 10000

// Coordinate struct which stores important coordinated
// related to solution
typedef struct coordinate {
    int x;      // row
    int y;      // column
} Coord;

// Global variables:
// - scaleFootrule is minimum sfr which is returned
// - finalRes[rank] stores the minimum path
// - tableNumZeros stores number of zeroes in solve matrix
// - solutionSize keeps track of the size of solution
// - markedRows and markedCols are boolean arrays determined by
//   rows and cols which are mared
double scaleFootrule = 0.0;
int finalRes[MAX_NUM_URL];
int tableNumZeros;
int solutionSize;
Coord solution[MAX_COORD];
bool markedRows[MAX_NUM_URL];
bool markedCols[MAX_NUM_URL];

double calcWeight(int id, int pos, int numUrls, int numFiles,
    int fileRank[MAX_NUM_FILES][MAX_NUM_URL], int fileNumUrls[MAX_NUM_FILES], 
    int invFileRank[MAX_NUM_FILES][MAX_NUM_URL], int totalNumUrls);
bool checkInFile(int id, int currFile, int fileNumUrls[MAX_NUM_URL], 
    int fileRank[MAX_NUM_FILES][MAX_NUM_URL]);
void findMinPath(double **solveMatrix, int size);
double findRowLowest(double **solveMatrix, int row, int size);
double findColLowest(double **solveMatrix, int col, int size);
void getBoolMatrix(bool **table, bool**copyTable, 
    double **solveMatrix, int size);
void optimise(double **solveMatrix, int size);
void bestRow(bool **table, int size);
void getResWeight(double weightMatrix[MAX_NUM_URL][MAX_NUM_URL], int size);
void convertColZero(bool **table, int size, int k);
int getLines(int size);
void refreshMarks(int size);
void getNextMatrix(double **solveMatrix, int size);


int main(int argc, char *argv[]) {

    // Need to read in at least one .txt file from cmd line
    if (argc < 2) {
        fprintf(stderr, "Usage: %s rankA.txt  rankD.txt", argv[0]);
        return EXIT_FAILURE;
    }

    // Initialise relevant arrays and sets for reading in information from files:
    // - fileNumUrls[] tracks number of urls read in per file
    // - fileRank[][] tracks the ranks of each url in each file given a rank
    // - invFileRank[][] is an inversed version of fileRank, used to find position of a url
    //   given a url string
    // - idToUrl[][] is a string array which stores the matching url at the urlId index
    // - str[] is used to read in url strings from each file
    // - urlToId is a set which stores all urls and their ids, used to keep track of unique
    //   urls
    int fileNumUrls[MAX_NUM_FILES] = {0};
    int fileRank[MAX_NUM_FILES][MAX_NUM_URL];
    int invFileRank[MAX_NUM_FILES][MAX_NUM_URL];
    char idToUrl[MAX_NUM_URL][MAX_URL_LENGTH];
    char str[MAX_URL_LENGTH];
    Set urlToId = setNew();

    // Assigning values to array whilst reading information file by file
    // ID corresponds to order of which url is found
    for (int i = 0; i < argc - 1; i++) {

        FILE *fp = fopen(argv[i + 1], "r");

        if (fp == NULL) {
            fprintf(stderr, "Can't open %s\n", argv[i + 1]);
            exit(EXIT_FAILURE);
        }

        // Go through each url stored in current file
        for (int j = 0; fscanf(fp, "%s", str) != EOF; j++) {

            char *urlRes = malloc(strlen(str) + 1);

            if (urlRes == NULL) {
                fprintf(stderr, "not enough memory\n");
                exit(EXIT_FAILURE);
            }

            urlRes = strcpy(urlRes, str);

            // Point of this is to append to set if unique url, does nothing but
            // return urlId if url already exists
            int urlId = setAddScore(urlToId, urlRes, 0.0);

            // Copy url into idToUrl char array
            strcpy(idToUrl[urlId], urlRes);

            // Stores rankings of each url each file
            fileRank[i][j] = urlId;

            // Get inverse file rank, add 1 to position since positions starts from 1, 2 ...
            // rather than from 0
            invFileRank[i][urlId] = j + 1;

            // Keep track of total number of urls in each file
            fileNumUrls[i]++;

            free(urlRes);
        }

        fclose(fp);
    }

    // Gets total number of unique urls from the set
    int totalNumUrls = setSize(urlToId);

    // 2D array storing each url's preferred rank
    double weightMatrix[MAX_NUM_URL][MAX_NUM_URL];

    // This is a pointer double matrix which copies the weightMatrix table
    // It is used for minimum pathing algorithm later to get the minimum weight
    double **solveMatrix = createCellMatrix(totalNumUrls, totalNumUrls);

    for (int urlId = 0; urlId < totalNumUrls; urlId++) {
        // Gets 2D array of weights
        for (int posRank = 1; posRank <= totalNumUrls; posRank++) {

            // Given a urlId, a given position, return sum of it's final weight
            weightMatrix[urlId][posRank - 1] = calcWeight(
                urlId, posRank, totalNumUrls, argc - 1, fileRank, 
                fileNumUrls, invFileRank, totalNumUrls);

            solveMatrix[urlId][posRank - 1] = weightMatrix[urlId][posRank - 1];

        }
    }

    // Finds minimum path given the current weight matrix
    // Restricting that only one weight can be taken from each
    // row and column
    findMinPath(solveMatrix, totalNumUrls);

    // Get result weight from solution array and update scaleFootrule value
    getResWeight(weightMatrix, totalNumUrls);

    // Print resulting lowest sfr
    printf("%.7lf\n", scaleFootrule);

    // Print order which gave that lowest value
    for (int currUrl = 0; currUrl < totalNumUrls; currUrl++) {
        printf("%s\n", idToUrl[finalRes[currUrl]]);
    }

    freeCellMatrix(solveMatrix);
    
    // Free set
    setFree(urlToId);

    return 0;
}

// Given a urlId and it's position, return a weight
double calcWeight(int id, int pos, int numUrls, int numFiles,
    int fileRank[MAX_NUM_FILES][MAX_NUM_URL], int fileNumUrls[MAX_NUM_FILES], 
    int invFileRank[MAX_NUM_FILES][MAX_NUM_URL], int totalNumUrls) {

    // Initialise iterative sum
    double sumWeight = 0.0;

    // Loop through all files
    for (int i = 0; i < numFiles; i++) {

        // If url is not in file, then go next value of i
        if (!checkInFile(id, i, fileNumUrls, fileRank)) {
            continue;
        }

        // Otherwise add on to sum
        sumWeight += fabs(
            (double)invFileRank[i][id] / (double)fileNumUrls[i]
            - (double)pos / (double)totalNumUrls
        );

    }

    // Return final sum
    return sumWeight;
}

// From the weight matrix and result array, updates scaleFootrule with minimum weight
void getResWeight(double weightMatrix[MAX_NUM_URL][MAX_NUM_URL], int size) {
    for (int i = 0; i < size; i++) {
        scaleFootrule += weightMatrix[finalRes[i]][i];
    }
}

// Checks if a url is in a file given a urlId and fileId
bool checkInFile(int id, int currFile, int fileNumUrls[MAX_NUM_URL], 
    int fileRank[MAX_NUM_FILES][MAX_NUM_URL]) {
    for (int i = 0; i < fileNumUrls[currFile]; i++) {
        if (fileRank[currFile][i] == id) {
            return true;
        }
    }
    return false;
}

// Given a matrix, find the best possible path which
// gives the minimum weight
void findMinPath(double **solveMatrix, int size) {

    // Find the minimum value in each row and minus it from each weight in the row
    for (int row = 0; row < size; row++) {
        double rowLowest = findRowLowest(solveMatrix, row, size);
        for (int col = 0; col < size; col++) {
            solveMatrix[row][col] -= rowLowest;
        }
    }

    // Find the minimum value in each column and minus it from each weight in the column
    for (int col = 0; col < size; col++) {
        double colLowest = findColLowest(solveMatrix, col, size);
        for (int row = 0; row < size; row++) {
            solveMatrix[row][col] -= colLowest;
        }
    }

    // Loop until number of cover lines is the same as the size of the matrix
    // Until number of coordinates stored in solution is equal to size, there
    // is no possible solution for minimum sum
    while (solutionSize < size) {

        // Retrieves mared rows and columns and also
        // a solution array of coordinates
        // If solutionSize = size, then there is an optimal solution
        optimise(solveMatrix, size);
        
        // If solution has not been found
        if (solutionSize < size) {
            getNextMatrix(solveMatrix, size);
        }

    }

    // Transfer solution coordinates to solution array
    for (int i = 0; i < size; i++) {
        finalRes[solution[i].y] = solution[i].x;
    }

    return;
}

// Given a row in a matrix, return the lowest value
double findRowLowest(double **solveMatrix, int row, int size) {
    double lowest = DEF_LOWEST;
    for (int i = 0; i < size; i++) {
        if (lowest > solveMatrix[row][i]) {
            lowest = solveMatrix[row][i];
        }
    }
    return lowest;
}

// Given a column in a matrix, return the highest value
double findColLowest(double **solveMatrix, int col, int size) {
    double lowest = DEF_LOWEST;
    for (int i = 0; i < size; i++) {
        if (lowest > solveMatrix[i][col]) {
            lowest = solveMatrix[i][col];
        }
    }
    return lowest;
}

// Updates values in bool matrices based on values in solve matrix
// For each coordinate in matrix:
// - true if weight is 0.0
// - false if otherwise
void getBoolMatrix(bool **table, bool **copyTable, double **solveMatrix, int size) {
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if (solveMatrix[i][j] == 0.0) {
                tableNumZeros++;
                table[i][j] = true;
                copyTable[i][j] = true;
            }
        }
    }
}

// Given a current state of a matrix, retrieve information on whether 
// a solution exists:
// - Checks for mared columns and mared rows
// - Retrieves a solution array of coordinates
void optimise(double **solveMatrix, int size) {

    // Reset number of zeroes on table
    tableNumZeros = 0;

    // Generate a normal table which saves original alignment
    // find solution entries and a dummy table which
    // eventually removes all true values
    bool **table = createBoolMatrix(size, size);
    bool **copyTable = createBoolMatrix(size, size);

    // Update boolean tables with true and false on whether coordinate
    // weight is 0.0
    getBoolMatrix(table, copyTable, solveMatrix, size);

    // Resets the marked columns, marked rows and number of coordinates
    // stored in solution
    refreshMarks(size);

    // Loop until number of zeroes in the copyTable is 0
    // i.e. entire matrix is filled with "false"
    // Upon each time bestRow() is called, we keep removing "true"
    // values until there are no more
    while (tableNumZeros > 0) {
        bestRow(copyTable, size);
    }

    // Initialise nonMarkedRowsArray -> stores rows values which are non-marked
    int nonMarkedRowsArray[MAX_NUM_URL];

    // Initialise nonMarkedRowsBool -> for each index matching the row:
    // - true if non-marked
    // - false if marked
    bool nonMarkedRowsBool[MAX_NUM_URL];

    // Keep track of number of non-marked
    int nonMarkedNum = 0;

    // Initialise all rows as unmarked
    for (int j = 0; j < size; j++) {
        nonMarkedRowsBool[j] = true;
    }

    // Remove marked bools
    for (int l = 0; l < solutionSize; l++) {
        nonMarkedRowsBool[solution[l].x] = false;
    }

    // Add unmarked rows to array
    for (int k = 0; k < size; k++) {
        if (nonMarkedRowsBool[k]) {
            nonMarkedRowsArray[nonMarkedNum] = k;
            nonMarkedNum++; 
        }
    }

    // Check variable on whether loop should continue or not
    bool check = true;

    // This while loop is what makes the solve function O(n^4) where n is 
    // number of unique urls
    // The optimise function is called in a while loop (O(n)) and the following
    // while -> for -> for nesting makes it O(n * n^3) = O(n^4)
    while (check) {

        check = false;
        // Find marked columns given unmarked rows
        // and unmarked 0s
        for (int i = 0; i < nonMarkedNum; i++) {

            // Copy unmarked row into a row boolean
            bool rowBool[MAX_NUM_URL];
            for (int n = 0; n < size; n++) {
                rowBool[n] = table[nonMarkedRowsArray[i]][n];
            }

            // If j'th column in current unmarked row is a 0
            // and it is not the column is unmarked: mark column
            // and while loops should continue
            for (int j = 0; j < size; j++) {
                if (rowBool[j] && !markedCols[j]){
                    markedCols[j] = true;
                    check = true;
                }
            }

        }

        // Goes through coordinates stored in solution
        for (int k = 0; k < solutionSize; k++) {
            
            // If current solution row considered is marked (i.e. not unmarked)
            // and the retrospective column is also marked:
            // - Add to unmarked/remove from effective marked rows
            // - Used to finalise marked rows after
            // - while loop continues
            if (!nonMarkedRowsBool[solution[k].x] && markedCols[solution[k].y]) {
                nonMarkedRowsBool[solution[k].x] = true;
                nonMarkedRowsArray[nonMarkedNum] = solution[k].x;
                nonMarkedNum++;
                check = true;
            }

        }

    }

    // Find marked rows based on unmarked rows
    // i.e. marked rows is the false values of nonmarkedrowsbool
    for (int i = 0; i < size; i++) {
        if (nonMarkedRowsBool[i]) {
            markedRows[i] = false;
        } else {
            markedRows[i] = true;
        }
    }

    // Free the copied table and original table
    freeBoolMatrix(copyTable);
    freeBoolMatrix(table);

    return;
}

// Refreshes the marked columns and rows size of 
// solution
void refreshMarks(int size) {
    for (int i = 0; i < size; i++) {
        markedCols[i] = false;
        markedRows[i] = false;
    }
    solutionSize = 0;
    return;
}

// Finds the row with the lowest amount of zeroes
// but excludes rows with no zeroes
void bestRow(bool **table, int size) {

    // Find row with lowest amount of zeroes
    int rowNumZero[MAX_NUM_URL];
    int minRow;
    int minCount = DEF_LOWEST;
    for (int i = 0; i < size; i++) {
        rowNumZero[i] = 0;
        for (int j = 0; j < size; j++) {
            if (table[i][j]) {
                rowNumZero[i]++;
            }
        }
        if (rowNumZero[i] < minCount && rowNumZero[i] > 0) {
            minRow = i;
            minCount = rowNumZero[i];
        }
    }

    // Traverse through the row with lowest amount of zeroes
    // and finds the first coordinate that is "true"
    // Saves that coordinate to "solution" and converts the rest of the row
    // and the solution's column to false
    bool check = false;
    for (int k = 0; k < size; k++) {
        
        if (table[minRow][k]) {
            table[minRow][k] = false;
            tableNumZeros--;
            // Making sure it is the first "zero" in the row
            if (!check) {
                solution[solutionSize].x = minRow;
                solution[solutionSize].y = k;
                solutionSize++;
                // Convert all current column to false
                convertColZero(table, size, k); 
                check = true;
            }
        }

    }

    return;

}

// Given a column and a table, flips all true values to false
void convertColZero(bool **table, int size, int k) {
    for (int l = 0; l < size; l++) {
        if (table[l][k]) {
            table[l][k] = false;
            tableNumZeros--;
        }
    }
}

// Given a current state of the solve matrix, finds the
// next best state to check for solution
void getNextMatrix(double **solveMatrix, int size) {

    // Initialise array which stores unmared coordinates
    Coord nonmared[MAX_COORD];

    // Keep track of number of entities in array
    int nonmaredSize = 0;

    // Default min value
    double min = DEF_LOWEST;

    // Loop through all coordinates of matrix
    // but ignores marked rows and columns
    for (int i = 0; i < size; i++) {
        if (markedRows[i]) {
            continue;
        }
        for (int j = 0; j < size; j++) {

            if (markedCols[j]) {
                continue;
            }

            // Update min value if current coordinate
            // is of smaller weight
            if (solveMatrix[i][j] < min) {
                min = solveMatrix[i][j];
            }

            // Add coordinate to nonmared array
            nonmared[nonmaredSize].x = i;
            nonmared[nonmaredSize].y = j;
            nonmaredSize++;

        }
    }

    // Minus the minimum value from all unmared coordinates
    for (int k = 0; k < nonmaredSize; k++) {
        solveMatrix[nonmared[k].x][nonmared[k].y] -= min;
    }

    // Add the minimum value onto all coordinates which are "mared"
    // doubly
    // i.e. if column l and row m are marked, then matrix[l][m] is doubly
    // mared
    for (int l = 0; l < size; l++) {
        for (int m = 0; m < size; m++) {
            if (markedRows[l] && markedCols[m]) {
                solveMatrix[l][m] += min;
            }
        }
    }

    return;
}