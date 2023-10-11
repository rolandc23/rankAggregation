// COMP2521 Assignment 2

// Written by: z5311913
// Date: 10/11/2022

// Given a set of files containing url rankings
// Returns the most "agreeable" rank of urls based on
// scaled footrule weighting score

#include <assert.h>
#include <ctype.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Set.h"

#define MAX_NUM_FILES 1000
#define MAX_NUM_URL 100
#define MAX_URL_LENGTH 1000

// Global variables:
// scaleFootrule - stores current best sfr score
// finalRes      - array which stores the combination of urls which gives current best sfr
// streak        - an iterative integer keeping track of how many iterations the
//                 current best sfr has held on for
double scaleFootrule = 100.0;
int finalRes[MAX_NUM_URL];
int streak;

// Url position information struct
// Given a url in a position, it also stores the weight
typedef struct urlPI {
    int rank;           // url's position for a test array
    double weight;      // weight of url given url's "rank" in test array
} urlPI;

double calcWeight(int id, int pos, int numUrls, int numFiles,
    int fileRank[MAX_NUM_FILES][MAX_NUM_URL], int fileNumUrls[MAX_NUM_FILES], 
    int invFileRank[MAX_NUM_FILES][MAX_NUM_URL], int totalNumUrls);
void mergeSortUrlPI(urlPI *u, int lo, int hi);
void mergeUrlPI(urlPI *u, int lo, int mid, int hi);
bool odd(int n);
void getFirstArray(int *currArray, 
    int totalNumUrls, urlPI urlInf[MAX_NUM_URL][MAX_NUM_URL]);
bool checkInFile(int id, int currFile, int fileNumUrls[MAX_NUM_URL], 
    int fileRank[MAX_NUM_FILES][MAX_NUM_URL]);
void findMin(int *array, int i, int length, 
    double prefUrlRank[MAX_NUM_URL][MAX_NUM_URL], int maxStreak);
void swap(int *i, int *j);
int factorial(int a);

int main(int argc, char *argv[]) {

    // Need to read in at least one .txt file from cmd line
    if (argc < 2) {
        fprintf(stderr, "Usage: %s rankA.txt  rankD.txt", argv[0]);
        return EXIT_FAILURE;
    }

    // Initialise relevant arrays and sets for reading in information from files:
    // - fileNumUrls[] tracks number of urls read in per file
    // - fileRank[][] tracks the ranks of each url in each file given a rank
    // - invFileRank[][] is an inversed version of fileRank, used to find position 
    //   of a url given a url string
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

            // Get inverse file rank, add 1 to position since positions starts 
            // from 1, 2 ... rather than from 0
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
    urlPI urlInf[MAX_NUM_URL][MAX_NUM_URL];

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

        // Convert information from prefUrlRank array into
        // urlInf array of urlPI
        // Keep information on rank for mergeSortUrlPIing algorithm
        for (int i = 0; i < totalNumUrls; i++) {
            urlInf[urlId][i].rank = i;
            urlInf[urlId][i].weight = prefUrlRank[urlId][i];
        }

        // Sort the urlInf[id] array from smallest to largest weight using mergesort
        mergeSortUrlPI(urlInf[urlId], 0, totalNumUrls - 1);

    }

    // currArray[] is the array which stores the current permuation that we are
    // checking for
    int currArray[MAX_NUM_URL];

    // This is what restricts the search size of the findMin function
    // If the current best sfr is the best for "maxStreak" amount of
    // iterations, then we return the result
    // maxStreak = totalNumUrls! / 2
    // We take into consideration that it does not always reduce the search,
    // but it is a very high possibility for it to do so
    int maxStreak = factorial(totalNumUrls) / 2;

    // To assist with reducing the amount of iterations we search, we get
    // a rather optimal first array which takes each url's best weighted ranking
    // This is to ensure that the best possible array is not too far from
    // the initial array
    getFirstArray(currArray, totalNumUrls, urlInf);

    // Traverses through all possible combinations until streak reaches maxStreak
    // or all combinations have been traversed through
    findMin(currArray, 0, totalNumUrls, prefUrlRank, maxStreak);

    // Print resulting lowest sfr
    printf("%.7lf\n", scaleFootrule);

    // Print order which gave lowest value
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
            (double) invFileRank[i][id] / (double) fileNumUrls[i]
            - (double) pos / (double) totalNumUrls
        );

    }

    // Return final sum
    return sumWeight;
}

// Merge sort on an array of urlPI
void mergeSortUrlPI(urlPI *u, int lo, int hi) {

    // Calculate mid point
    int mid = (lo + hi) / 2;

    // Check if we have reached end of array
    if (hi <= lo) {
        return;
    }

    // Recursively sort lower half, sort upper half then
    // merge all urlPIs again
    mergeSortUrlPI(u, lo, mid);
    mergeSortUrlPI(u, mid + 1, hi);
    mergeUrlPI(u, lo, mid, hi);

    return;
}

// Merges 2 arrays together in sorted order
void mergeUrlPI(urlPI *u, int lo, int mid, int hi) {

    // Get number of items in the urlPI array for storage assignment
    // on tmp
    int nitems = hi - lo + 1;

    urlPI *tmp = malloc(nitems * sizeof(urlPI));

    if (tmp == NULL) {
        fprintf(stderr, "not enough memory\n");
        exit(EXIT_FAILURE);
    }

    int i = lo,
        j = mid + 1,
        k = 0;

    // Go through both segments and copy onto tmp
    while (i <= mid && j <= hi) {
        if (u[i].weight > u[j].weight) {
            tmp[k++] = u[j++];
        } else {
            tmp[k++] = u[i++];
        }
    }

    // Copy lower half
    while (i <= mid) {
        tmp[k++] = u[i++];
    }

    // Copy upper half
    while (j <= hi) {
        tmp[k++] = u[j++];
    }

    // Copy back to main array
    for (i = lo, k = 0; i <= hi; i++, k++) {
        u[i] = tmp[k];
    }

    free(tmp);

    return;
}

// Checks if a number is odd
bool odd(int n) {
    if (n % 2 == 0) {
        return false;
    }
    return true;
}

// Given a initial values, gets the first array to check
void getFirstArray(int *currArray, int totalNumUrls,  
    urlPI urlInf[MAX_NUM_URL][MAX_NUM_URL]) {
    
    // Initialise bool array which keeps track of which positions are taken
    bool notVacant[totalNumUrls];

    // Assign all of bool to false, becomes true if position is taken
    for (int i = 0; i < totalNumUrls; i++) {
        notVacant[i] = false;
    }

    // Loop through urls and get their best positions
    for (int curr = 0; curr < totalNumUrls; curr++) {

        // Loop through each url's preferred position to find next
        // vacant position
        for (int j = 0; j < totalNumUrls; j++) {
            int checkRank = urlInf[curr][j].rank;
            // If vacant, add to current array, update curr comb array, 
            // reverse vacancy condition to false
            if (!notVacant[checkRank]) {
                currArray[checkRank] = curr;
                notVacant[checkRank] = true;
                break;
            }
        }
    }

    return;
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

// Recursively goes through possible combinations of array until 
// maxStreak is reached or all combinations have been iterated through
void findMin(int *array, int i, int length, 
    double prefUrlRank[MAX_NUM_URL][MAX_NUM_URL], int maxStreak) {
        
    double tmp = 0.0;

    // If array is completely filled, check for sfr score
    if (i == length) {

        // Sum all weights of urls in their positions
        for(int l = 0; l < length; l++) {
            tmp += prefUrlRank[array[l]][l];
        }

        // If weight is lower than current best, update
        if (tmp < scaleFootrule) {
            scaleFootrule = tmp;
            // Copy array onto result array
            for(int k = 0; k < length; k++) {
                finalRes[k] = array[k];
            }
            // Reset streak
            streak = 0;
        } else {
            // Otherwise increase streak
            streak++;
        }

        return;
    }

    // Continue building next check array if it's not complete
    for (int j = i; j < length && streak <= maxStreak; j++) { 
        swap(&array[j], &array[i]);
        findMin(array, i + 1, length, prefUrlRank, maxStreak);
        swap(&array[j], &array[i]);
    }

    return;
}

// Given 2 int array pointers, swap their values
void swap(int *i, int *j) {
    int tmp = *i;
    *i = *j;
    *j = tmp;
    return;
}

// Given an integer, return the factorial of that integer
int factorial(int a) {
    int res = a;
    for (int i = a - 1; i > 0; i--) {
        res *= i;
    }
    return res;
}