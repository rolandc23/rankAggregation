// z5311913
// November 14 2022

// Adapted from FlightDb.c in assignment 1
// Contains functions which are used for managing and searching
// a database of words

#include <assert.h>
#include <ctype.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Tree.h"
#include "Record.h"
#include "List.h"
#include "SearchDb.h"

int compareByAlphabetical(Record r1, Record r2);

// Comparison function which sorts records alphabetically
int compareByAlphabetical(Record r1, Record r2) {
    int cmp = strcmp(RecordGetWord(r1), RecordGetWord(r2));
    return cmp;
}

// Creates new search database
SearchDb DbNew(void) {

    SearchDb db = malloc(sizeof(*db));

    if (db == NULL) {
        fprintf(stderr, "error: out of memory\n");
        exit(EXIT_FAILURE);
    }

    // Create tree that sorts record's  words alphabetically
    db->byAlphabetical = TreeNew(compareByAlphabetical);

    return db;
}

// Free a database
void DbFree(SearchDb db) {
    TreeFree(db->byAlphabetical, true);
    free(db);
}

// Inserts a record into db given db and a record
// Returns true if inserted
// Returns false if not inserted
bool DbInsertRecord(SearchDb db, Record r) {
    if (TreeInsert(db->byAlphabetical, r)) {
        return true;
    } else {
        return false;
    }
}

// Given a word, search database for the record containing the word
// Returns record of word
// Returns NULL if word is not in search db
Record DbFindWord(SearchDb db, char *word) {

    // Create dummy set
    Set dummyList = setNew();
    // Create dummy record containing "word"
    Record dummy = RecordNew(word, dummyList);

    // Search for record
    Record r = TreeSearch(db->byAlphabetical, dummy);

    // Free dummy set and dummy record
    setFree(dummyList);
    RecordFree(dummy);
    
    return r;
}