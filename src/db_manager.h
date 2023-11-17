#ifndef DB_MANAGER_H
#define DB_MANAGER_H

#include <stdlib.h>
#include <stdio.h>

#define INITIAL_HASH_TABLE_SIZE 16

/* Data Structures */

typedef struct {
    char *id;       // document ID
    char *content;  // json
} Document;

typedef struct HashEntry {
    char *key;
    Document *value;
    struct HashEntry *next; // to manage collisions with chaining
} HashEntry;

typedef struct {
    char *id;
    int size;
    HashEntry **buckets; // array of pointers to HashEntry
} HashTable;

typedef struct {
    Document *documents; // dynamic array of documents
    HashTable *hashTable; // HashTable for the collection
    int size;            // number of documents currently stored
    int capacity;        // current capacity of the array
} Collection;

/* Functions */

HashTable *create_hash_table();
Collection *create_collection();

#endif // DB_MANAGER_H