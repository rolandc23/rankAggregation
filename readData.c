// z5311913
// November 14 2022

// Mainly contains functions which are used to read data from files
// and convert them to respective interpretations such as arrays, 
// graphs or sets

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>

#include "Graph.h"
#include "Set.h"
#include "readData.h"
#include "Record.h"
#include "SearchDb.h"

#define INVERTED "invertedIndex.txt"
#define COLLECTION "collection.txt"
#define PRLIST "pageRankList.txt"
#define MAX_URL_LENGTH 100
#define MAX_LINE_LENGTH 1000
#define FAIL -1

// Given a collection.txt file, reads in all urls
Set getCollection() {

    // Open collection.txt
    FILE *fp = fopen(COLLECTION, "r");

    // Check valid file exists
    if (fp == NULL) {
        fprintf(stderr, "Can't open %s\n", COLLECTION);
        exit(EXIT_FAILURE);
    }

    // Create new set to store urls into and character string to read urls
    Set urlList = setNew();
    char url[MAX_URL_LENGTH];

    // Scan in each url and add to set
    while (fscanf(fp, "%s", url) != EOF) {
        char *tmp = malloc(strlen(url) + 1);

        if (tmp == NULL) {
            fprintf(stderr, "not enough memory\n");
            exit(EXIT_FAILURE);
        }

        tmp = strcpy(tmp, url);
        setInsert(urlList, tmp, 0.0);
        free(tmp);
    }

    // Close file
    fclose(fp);

    return urlList;
}

// Converts given set into graph, returning the graph
Graph getGraph(Set s) {

    // Create new graph give number of total urls
    Graph g = GraphNew(s->nitems);

    // Initialise string 
    char *urlF;

    for (Node tmp = s->head; tmp != NULL; tmp = tmp->next) {

        // Assign url string, 5 extra size for extra characters
        // added on sprintf
        urlF = calloc(strlen(tmp->url) + 5, sizeof(char));
        sprintf(urlF, "%s.txt", tmp->url);

        // Read the url file
        FILE *urlInfo = fopen(urlF, "r");

        // Check valid file exists
        if (urlInfo == NULL) {
            fprintf(stderr, "Can't open %s\n", urlF);
            exit(EXIT_FAILURE);
        }

        // Add outgoing links and edges
        Vertex v = tmp->id;
        addEdges(g, s, v, urlInfo);

        // Close file
        fclose(urlInfo);

        // Free urlF
        free(urlF);

    }

    return g;
}

// Given a url file, add outgoing links ("edges") to the graph vertex
void addEdges(Graph g, Set s, Vertex source, FILE *urlInfo) {

    // Initialise edge
    Edge e;
    e.v = source;

    // Read the url.txt file for outgoing links, 
    // stopping at end of section 1
    char *urlRes;
    char str[MAX_LINE_LENGTH];

    while (fscanf(urlInfo, "%s", str) != EOF) {

        // Check for opening 2 strings of section 1
        if (strcmp(str, "Section-1") == 0
            || strcmp(str, "#start") == 0) {
            continue;
        }

        // Check for closing string of section 1
        if (strcmp(str, "#end") == 0) {
            break;
        }

        urlRes = malloc(strlen(str) + 1);

        if (urlRes == NULL) {
            fprintf(stderr, "not enough memory\n");
            exit(EXIT_FAILURE);
        }

        urlRes = strcpy(urlRes, str);
        Vertex vRes = getvID(s, urlRes);

        // Do not want self-loops
        if (source == vRes) {
            free(urlRes);
            continue;
        }

        // Assign outgoing link on edge and insert
        e.w = vRes;
        GraphInsertEdge(g, e);
        
        free(urlRes);

    }

    return;
}

// Finds the vertex ID corresponding to a url
// Returns FAIL if failed
Vertex getvID(Set s, char *url) {
    for (Node tmp = s->head; tmp != NULL; tmp = tmp->next) {
        if (strcmp(url, tmp->url) == 0) {
            return tmp->id;
        }
    }
    return FAIL;
}

// Reads in invertedIndex.txt and creates
// a search database which uses AVL tree insertion
SearchDb getInverted() {

    // Open invertedIndex.txt
    FILE *fp = fopen(INVERTED, "r");

    // Check valid file exists
    if (fp == NULL) {
        fprintf(stderr, "Can't open %s\n", INVERTED);
        exit(EXIT_FAILURE);
    }

    // Create new db
    SearchDb db = DbNew();
    char line[MAX_LINE_LENGTH];

    // Go through each line of invertedIndex.txt
    while (fgets(line, MAX_LINE_LENGTH + 1, fp) != NULL) {

        // Read word
        char *buffer = line;
        char word[MAX_WORD_LENGTH];
        char url[MAX_WORD_LENGTH];
        int pos;

        // Create new set to store list of urls
        Set urlList = setNew();

        // Scan first item on the line, which should be a "word"
        sscanf(buffer, "%99s%n", word, &pos);
        buffer += pos;

        // Continue scanning line for "urls"
        while (sscanf(buffer, "%99s%n", url, &pos) == 1) {
            
            // Malloc character string and copy url that was read
            char *getUrl = malloc(strlen(url) + 1);

            if (getUrl == NULL) {
                fprintf(stderr, "not enough memory\n");
                exit(EXIT_FAILURE);
            }
            
            getUrl = strcpy(getUrl, url);

            // Insert into set
            setInsert(urlList, getUrl, getPageRank(getUrl));

            // Iterate buffer to read following url
            buffer += pos;

            // Free string
            free(getUrl);

        }
        
        // Create a new record given the word and the urlList set
        Record r = RecordNew(word, urlList);

        // Insert record into db
        DbInsertRecord(db, r);

        // Free urlList set
        setFree(urlList);

    }

    // Close file
    fclose(fp);

    return db;
}

// Return weight of given page url
// Return 0 if url does not exist
double getPageRank(char *url) {

    // Open pageRankList.txt
    FILE *fp = fopen(PRLIST, "r");

    // Check if file exists
    if (fp == NULL) {
        fprintf(stderr, "Can't open %s\n", PRLIST);
        exit(EXIT_FAILURE);
    }

    // Initialise read-in string for each line
    char line[MAX_LINE_LENGTH];

    while (fgets(line, MAX_LINE_LENGTH + 1, fp) != NULL) {

        // Read word, outlink, weight
        char word[MAX_WORD_LENGTH];
        int outlink;
        double weight;
        sscanf(line, "%s %d %lf", word, &outlink, &weight);

        // If url matches the "url" search, close file and return weight
        if (strcmp(url, word) == 0) {
            fclose(fp);
            return weight;
        }

    }

    // Close file
    fclose(fp);

    return 0;
}