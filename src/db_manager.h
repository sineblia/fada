#ifndef DB_MANAGER_H
#define DB_MANAGER_H

#include <stdlib.h>
#include <stdio.h>

#define INITIAL_HASH_TABLE_SIZE 16

/* Data Structures */

typedef struct HashEntry HashEntry;

typedef struct {
    char *id;       // document ID
    HashEntry *hash_id; // hash ID generated from the original ID
    char *content;  // json
} Document;

typedef struct HashEntry {
    char *key; // original document _id
    unsigned long hash; // hash value calculated from the ID
    Document *value; // doc reference
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
    char *id; // collection ID
    int size;            // number of documents currently stored
    int capacity;        // current capacity of the array
} Collection;

/* Functions */

HashTable *create_hash_table();
unsigned long hash_function(const char *str);
HashEntry *create_hash_entry(char *key, unsigned long hash, Document *value);
Collection *create_collection();
char *generate_unique_id();
Document *create_document(const char *content);

void free_hash_table(HashTable *table);
void free_hash_entry(HashEntry *entry);
void free_collection(Collection *collection);
void free_document(Document *doc);

#endif // DB_MANAGER_H