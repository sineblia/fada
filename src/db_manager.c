/*
 * Copyright (c) 2023, Simone Bellavia <simone.bellavia@live.it>.
 * All rights reserved.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

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
    Document **documents; // dynamic array of documents
    HashTable *hashTable; // HashTable for the collection
    char *id; // collection ID
    int size;            // number of documents currently stored
    int capacity;        // current capacity of the array
} Collection;

/* Collection functions */

HashTable *create_hash_table(){
    HashTable *table = malloc(sizeof(HashTable));
    if(!table){
        return NULL;
    }

    table->size = INITIAL_HASH_TABLE_SIZE;
    table->buckets = malloc(sizeof(HashEntry*) * table->size);

    if(!table->buckets){
        free(table);
        return NULL;
    }

    // initializes each bucket to NULL
    for (int i = 0; i < table->size; i++){
        table->buckets[i] = NULL;
    }

    return table;
};

/*

HASH FUNCTION

The generated hashes should be uniformly distributed in the hash space to minimize collisions.
The same key should always produce the same hash value. The hashing function should be fast 
enough to compute. For these reasons, here we're using Dan Bernstein's "djb2" hashing algorithm.

*/

unsigned long hash_function(const char *str){
    unsigned long hash = 5381;
    int c;

    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c; // hash * 33 + c
    }

    return hash;
}

/*

HASH ENTRY

The HashEntry includes both the original key (document ID) and the hash value computed from that key. 
This approach allows to maintain both quick access to documents (through the hash) and the ability to 
handle collisions and maintain data integrity (through the original key and linked list management).

*/

HashEntry *create_hash_entry(char *key, unsigned long hash, Document *value) {
    HashEntry *entry = malloc(sizeof(HashEntry));
    if (!entry) {
        return NULL;
    }

    entry->key = strdup(key);
    entry->hash = hash;
    entry->value = value;
    entry->next = NULL;

    return entry;
}

Collection *create_collection(){
    Collection *collection = malloc(sizeof(Collection));
    if(!collection){
        return NULL;
    }
    
    // Creates the hash table
    collection->hashTable = create_hash_table();
    if(!collection->hashTable){
        free(collection);
        return NULL;
    }

    // Initializes the documents array
    collection->documents = NULL;
    collection->size = 0;
    collection->capacity = 0;
    return collection;
}

/* Document CRUD functions */

char *generate_unique_id() {
    static int counter = 0;
    time_t now = time(NULL);
    char *id = malloc(32); // enough size for the ID

    if (id) {
        snprintf(id, 32, "%lx_%d", (long)now, counter++);
    }

    return id; // who calls this function will have to free the memory
}

Document *create_document(const char *content) {
    Document *doc = malloc(sizeof(Document));
    if (!doc) return NULL; // malloc fail

    doc->id = strdup(generate_unique_id());
    if(!doc->id) {
        free(doc); // clean in case of error
        return NULL;
    }

    doc->content = strdup(content);
    if (!doc->content) {
        free(doc->id); // clean in case of error
        free(doc);
        return NULL;
    }

    unsigned long hash_value = hash_function(doc->id);
    doc->hash_id = create_hash_entry(doc->id, hash_value, doc);
    if (!doc->hash_id) {
        free(doc->content);
        free(doc->id);
        free(doc);
        return NULL;
    }

    // Saving the document on disk
    char filename[256];
    snprintf(filename, sizeof(filename), "%s.json", doc->id);
    FILE *file = fopen(filename, "w");
    if(file) {
        fprintf(file, "%s", doc->content);
        fclose(file);
    } else {
        // if we arrived here, then we have an error while opening the file
        // we handle it
        free(doc->content);
        free(doc->id);
        free(doc);
        return NULL;
    }

    return doc;
}

char *read_document(const char* id){

    char filename[256];
    snprintf(filename, sizeof(filename), "%s.json", id);

    FILE *file = fopen(filename, "r");

    // We should manage the case when the file doesn't exist
    if (file == NULL){
        return NULL;
    }

    // Find the file dimensions
    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);

    // Allocate the memory for the file's contents
    char *content = malloc(length + 1);
    if (content == NULL){
        // Handle the error of memory allocation
        fclose(file);
        return NULL;
    }

    // Read the file contents
    fread(content, 1, length, file);
    content[length] = '\0'; // We make sure that the string correctly reach its end

    fclose(file);
    return content; // Who calls the function will be responsible of freeing this memory

}

bool update_document(const char *id, const char *new_content){
    char filename[256];
    // snprintf(filename, sizeof(filename), "%s.json", id);

    FILE *file = fopen(filename, "w");
    if (file == NULL){
        return false;
    }

    fprintf(file, "%s", new_content);
    fclose(file);

    return true;
}

bool delete_document(const char *id){
    char filename[256];
    snprintf(filename, sizeof(filename), "%s.json", id);

    if (remove(filename) == 0) {
        printf("Deleted successfully\n");
        return true;
    } else {
        printf("Unable to delete the file\n");
        return false;
    }
}

/* Free Memory Functions */
void free_hash_entry(HashEntry *entry) {
    if (entry == NULL) return;

    free(entry->key);
    
    free(entry);
}

void free_document(Document *doc) {
    if (doc == NULL) return;

    free(doc->id);
    free(doc->content);
    free(doc);
}

void free_hash_table(HashTable *table) {
    if (table == NULL) return;

    for (int i = 0; i < table->size; i++) {
        HashEntry *entry = table->buckets[i];
        while (entry != NULL) {
            HashEntry *temp = entry;
            entry = entry->next;
            free_hash_entry(temp); 
        }
    }

    free(table->buckets);
    free(table);
}

void free_collection(Collection *collection) {
    if (collection == NULL) return;

    for (int i = 0; i < collection->size; i++) {
        free_document(collection->documents[i]);
    }

    free(collection->documents);
    free(collection);
}