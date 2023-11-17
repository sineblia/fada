/*
 * Copyright (c) 2023, Simone Bellavia <simone.bellavia@live.it>.
 * All rights reserved.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

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

bool add_document_to_collection(Collection *collection, Document *document){
    if (collection->size >= collection->capacity){
        // Dynamic expansion
        int new_capacity = collection->capacity * 2;
        Document * new_array = realloc(collection->documents, sizeof(Document) * new_capacity);
        if (!new_array) return false; // Reallocation fails

        collection->documents = new_array;
        collection->capacity = new_capacity;
    }

    collection->documents[collection->size++] = *document;
    return true;
}

/* Document CRUD functions */

Document *create_document(const char *id, const char *content) {
    Document *doc = malloc(sizeof(Document));
    if (!doc) return NULL; // malloc fail

    doc->id = strdup(id);
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