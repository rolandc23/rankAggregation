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

#define MAX_NUM_FILES 1000
#define MAX_NUM_URL 100
#define MAX_URL_LENGTH 1000

// Global variables for "minimum" scaleFootrule and it's respective url ranking
double scaleFootrule = 6969.0;
int finalRes[MAX_NUM_URL];
int iteration = 0;

typedef struct urlPI {
    int rank;           // url's position for a test array
    double weight;      // weight of url given url's "rank" in test array
} urlPI;

double calcWeight(int id, int pos, int numUrls, int numFiles,
    int fileRank[MAX_NUM_FILES][MAX_NUM_URL], int fileNumUrls[MAX_NUM_FILES], 
    int invFileRank[MAX_NUM_FILES][MAX_NUM_URL], int totalNumUrls);
// void sort(urlPI *u, int len);
void swap(int *i, int *j);
// bool odd(int n);
// void getMinScaleFoot(urlPI urlInf[MAX_NUM_URL][MAX_NUM_URL], int totalNumUrls, 
//     int *currArray, int *currComb);
// void getNextIteration(int *currArray, 
    // int totalNumUrls, int urlId, int it, urlPI urlInf[MAX_NUM_URL][MAX_NUM_URL], int *currComb);
bool checkInFile(int id, int currFile, int fileNumUrls[MAX_NUM_URL], 
    int fileRank[MAX_NUM_FILES][MAX_NUM_URL]);
void permute(int *array, int i, int length, double prefUrlRank[MAX_NUM_URL][MAX_NUM_URL]);

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

    // Initialise an array of "urlPI" which stores rank and weight
    // Point of this is to reduce our search to the "best half" of each
    // url's preferred positions
    // urlPI urlInf[MAX_NUM_URL][MAX_NUM_URL];

    // 2D array storing each url's preferred rank
    double prefUrlRank[MAX_NUM_URL][MAX_NUM_URL];
    for (int urlId = 0; urlId < totalNumUrls; urlId++) {
        // Gets 2D array of weights
        for (int posRank = 1; posRank <= totalNumUrls; posRank++) {
            // Given a urlId, a given position, return sum of it's final weight
            prefUrlRank[urlId][posRank - 1] = calcWeight(
                urlId, posRank, totalNumUrls, argc - 1, fileRank, 
                fileNumUrls, invFileRank, totalNumUrls);
        }
        

    }

    int currArray[MAX_NUM_URL];
    for (int i = 0; i < totalNumUrls; i++) {
        currArray[i] = i;
    }

    permute(currArray, 0, totalNumUrls, prefUrlRank);

    // Print resulting lowest sfr
    printf("%.7lf after %d iterations\n", scaleFootrule, iteration);

    // Print order which gave that lowest value
    for (int currUrl = 0; currUrl < totalNumUrls; currUrl++) {
        printf("%s\n", idToUrl[finalRes[currUrl]]);
    }

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

// Simple iterative sort
// void sort(urlPI *u, int len) {
//     for (int i = 0; i < len; i++) {
//         for (int j = i + 1; j < len; j++) {
//             // Swap if out of position
//             if (u[i].weight > u[j].weight) {
//                 swap(&u[i], &u[j]);
//             }
//         }
//     }
// }

// Simple swap between 2 urlPI pointers
void swap(int *i, int *j) {
    int tmp = *i;
    *i = *j;
    *j = tmp;
    return;
}

// Checks if a number is odd
bool odd(int n) {
    if (n % 2 == 0) {
        return false;
    }
    return true;
}

// For each given check array, calculates the scaleFootrule value for it
// If it is the new lowest, update
// Otherwise ignore
// void getMinScaleFoot(urlPI urlInf[MAX_NUM_URL][MAX_NUM_URL], int totalNumUrls, 
//     int *currArray, int *currComb) {
    
//     // Initialise weight sum
//     double currWeight = 0.0;

//     // Loop through all url's and their positions to get final weight
//     for (int i = 0; i < totalNumUrls; i++) {
//         currWeight += urlInf[currArray[i]][currComb[i]].weight;
//     }

//     // If new weight is lower than current best, update
//     if (currWeight < scaleFootrule) {
//         // Also update the array of the final result
//         for (int j = 0; j < totalNumUrls; j++) {
//             finalRes[j] = currArray[j];
//         }
//         scaleFootrule = currWeight;
//     }

//     return;

// }

// Given a urlId and iteration, get's the next check array
// void getNextIteration(int *currArray, int totalNumUrls, int urlId, int it, 
//     urlPI urlInf[MAX_NUM_URL][MAX_NUM_URL], int *currComb) {
    
//     // Initialise all positions as vacant
//     for (int pos = 0; pos < totalNumUrls; pos++) {
//         vacant[pos] = true;
//     }

//     // Now we define the starting point
//     currArray[urlInf[urlId][it].rank] = urlId;
//     vacant[urlInf[urlId][it].rank] = false;
//     currComb[urlId] = it;

//     // Initialise curr as urlId + 1
//     int curr = urlId + 1;

//     // If urlId is last id, go to 0
//     if (curr == totalNumUrls) {
//         curr = 0;
//     }

//     // Then we traverse through other urls and get their best positions
//     // given the starting point
//     while (curr != urlId) {

//         // Loop through each url's preferred position to find next
//         // vacant position
//         for (int j = 0; j < totalNumUrls; j++) {
//             int checkRank = urlInf[curr][j].rank;
//             // If vacant, add to current array, update curr comb array, 
//             // reverse vacancy condition to false
//             if (vacant[checkRank]) {
//                 currArray[checkRank] = curr;
//                 vacant[checkRank] = false;
//                 currComb[curr] = j;
//                 break;
//             }
//         }

//         curr++;

//         // If curr goes out of array bound, go back to 0
//         if (curr == totalNumUrls) {
//             curr = 0;
//         }
        
//     }

//     return;
// }

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

//Permutes the order the positions and calculates the smallest scaled foot rule
void permute(int *array, int i, int length, double prefUrlRank[MAX_NUM_URL][MAX_NUM_URL])
 { 

	double temp = 0.0;
	int j, k, l;

	//printf("length:%d\n", length);
  	if (i == length){
        iteration++;
  		for(l = 0; l < length; l++) {
  			temp += prefUrlRank[array[l]][l];
  		}

     	if(temp < scaleFootrule) {
     		scaleFootrule = temp;
     		for(k = 0; k < length; k++) {
     				finalRes[k] = array[k];     			
     		}

     	}
    	return;
  	}
	for (j = i; j < length; j++) { 
 		swap(&array[i], &array[j]);
		permute(array,i+1, length, prefUrlRank);
        swap(&array[i], &array[j]);
	}
	return;
}