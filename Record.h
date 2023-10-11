// Interface to the Search Record ADT
// Adapted from COMP2521 assignment 1 2022

// !!! DO NOT MODIFY THIS FILE !!!

#ifndef RECORD_H
#define RECORD_H

#define MAX_WORD_LENGTH 1000

#include <stdio.h>
#include <stdbool.h>
#include "Set.h"

typedef struct record *Record;

struct record {
    char *word;     // word which is stored
    Set pageList;   // set of pages which contains the word
};

/**
 * Creates a record with the given flight number, departure airport,
 * arrival airport, departure time (consisting of day, hour, and minute)
 * and flight duration (in minutes).
 * Returns NULL if any of these are invalid.
 */
Record RecordNew(char *word, Set pageList);

/**
 * Frees all memory associated with the given record
 */
void RecordFree(Record r);

/**
 * Returns the word in the given record.
 */
char *RecordGetWord(Record r);

/**
 * Returns the list in the given record.
 */
Set RecordGetList(Record r);

/**
 * Displays the record's word 
 */
void RecordShow(Record r);

#endif
