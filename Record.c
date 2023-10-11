// z5311913
// November 14 2022

// Implementation of the Search Record ADT
// Adapted from assignment 1 2521

// !!! DO NOT MODIFY THIS FILE !!!

#include <assert.h>
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

// Create new record given a word and a set containing urls
Record RecordNew(char *word, Set urlList) {

    // Word is too long
    if (strlen(word) > MAX_WORD_LENGTH) {
        fprintf(stderr, "error: word '%s' is too long\n",
                word);
        return NULL;
    }

    Record r = malloc(sizeof(*r));

    if (r == NULL) {
        fprintf(stderr, "error: out of memory\n");
        exit(EXIT_FAILURE);
    }

    r->word = malloc(strlen(word) + 1);

    if (r->word == NULL) {
        fprintf(stderr, "not enough memory\n");
        exit(EXIT_FAILURE);
    }
    
    r->word = strcpy(r->word, word);
    r->pageList = setNew();
    
    Node tmp = urlList->head;
    while (tmp != NULL) {
        setInsert(r->pageList, tmp->url, tmp->w);
        tmp = tmp->next;
    }
    
    return r;
}

// Free a given record, its pagelist and word if they exist
void RecordFree(Record r) {
    if (r != NULL) {
        setFree(r->pageList);
        free(r->word);
    }
    free(r);
}

// Return the word of a record
char *RecordGetWord(Record r) {
    return r->word;
}

// Return the set list of urls in a record
Set RecordGetList(Record r) {
    return r->pageList;
}

// Displays a record's word
void RecordShow(Record r) {
    printf("%s", r->word);
}

