// COMP2521 Assignment 2

// Written by: z5311913
// Date: 8/11/2022

#include <assert.h>
#include <ctype.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "SearchDb.h"
#include "Record.h"
#include "Tree.h"
#include "readData.h"
#include "MergeSort.h"
#include "Set.h"
#include "List.h"

#define MAX_NUM_WORD 1000
#define MAX_DISPLAY_URL 30

int main(int argc, char *argv[]) {
    // argc is the number of command-line arguments, which includes the
    // program name
    // argv is the array of command-line arguments

    if (argc < 2) {
        fprintf(stderr, "Usage: %s word1 word2 ...", argv[0]);
        return EXIT_FAILURE;
    }

    // Initialise string array to store words from cmd line
    char words[MAX_NUM_WORD][MAX_WORD_LENGTH];

    // Initialise counter for number of words being searched
    int numWords = argc - 1;

    for (int i = 1; i <= numWords; i++) {
        strcpy(words[i - 1], argv[i]);
    }

    // Create database from invertedIndex.txt
    SearchDb db = getInverted();

    // Initialise record and firstWord, first word tracks the location
    // of first valid word that exists in db
    int firstWord = 0;
    Record init;

    // Find initial valid word existing in db and return record of it
    // The urlList of this record is what we will be using to keep track of
    // url scores for our final result
    for (int i = 0; i < numWords; i++) {

        init = DbFindWord(db, words[i]);

        // Go next word if not found
        if (init == NULL) {
            continue;
        }

        firstWord = i;
        break;

    }

    // If no valid words, exit program
    if (init == NULL) {
        RecordFree(init);
        DbFree(db);
        return 0;
    }

    // Loop through all other search words
    for (int j = firstWord; j < numWords; j++) {
        
        // Find current word and return record
        Record r = DbFindWord(db, words[j]);
        
        // If word does not exist, free record and continue
        if (r == NULL) {
            RecordFree(r);
            continue;
        }

        // Loop through the page list of given record
        Node tmp = r->pageList->head;
        while (tmp != NULL) {

            // Add to score of url into the initial word's pagelist
            // which tracks how many times a url has been matched with a search word
            setAddScore(init->pageList, tmp->url, tmp->w);
            tmp = tmp->next;

        }

        free(tmp);

    }

    // Sort the page list according to priority of:
    // 1) "searchScore"
    // 2) "pageRank"
    // 3) Alphabetical order
    setMergeSort(init->pageList);

    // Print highest 30 searches
    Node curr = init->pageList->head;
    int counter = 0;
    while (curr != NULL && counter < MAX_DISPLAY_URL)    {
        printf("%s\n", curr->url);
        curr = curr->next;
        counter++;
    }

    // Free search db
    DbFree(db);

    return 0;
}





