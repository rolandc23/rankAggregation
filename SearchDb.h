#ifndef SEARCHDB_H
#define SEARCHDB_H

#include <stdio.h>
#include <stdbool.h>
#include "Set.h"
#include "Record.h"
#include "Tree.h"

typedef struct searchDb *SearchDb;

struct searchDb {
    Tree byAlphabetical;
};

// Creates new search database
SearchDb DbNew(void);

// Free a database
void DbFree(SearchDb db);

// Inserts a record into db given db and a record
// Returns true if inserted
// Returns false if not inserted
bool DbInsertRecord(SearchDb db, Record r);

// Given a word, search database for the record containing the word
// Returns record of word
// Returns NULL if word is not in search db
Record DbFindWord(SearchDb db, char *word);


#endif