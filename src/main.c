/*
 * Copyright (c) 2023, Simone Bellavia <simone.bellavia@live.it>.
 * All rights reserved.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/* Basic data structures */
typedef struct {
    char *id;       // Unique document ID
    char *content;  // Document contents (JSON)
} Document;

typedef struct {
    Document *documents; // Dynamic array of documents
    int size;            // Number of documents currently stored
    int capacity;        // Current capacity of the array
} Collection;

/* Collection functions */

Collection *create_collection(int initial_capacity){
    Collection *collection = malloc(sizeof(Collection));
    if(!collection) return NULL;

    collection->documents = malloc(sizeof(Document) * initial_capacity);
    
    if(!collection->documents){
        free(collection);
        return NULL;
    }

    collection->size = 0;
    collection->capacity = initial_capacity;
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

int main() {
    char input[256];

    while (true) {
        printf("db> ");
        
        if (fgets(input, sizeof(input), stdin) == NULL) break;

        // Rimuovi il newline dall'input
        input[strcspn(input, "\n")] = 0;

        if (strcmp(input, "create") == 0) {
            printf("What do you want to create?\n");
            printf("1. A new collection\n");
            printf("2. A new document\n");
            
            printf("Please enter your choice: ");
            if (fgets(input, sizeof(input), stdin) == NULL) break;
            
            // Rimuovi di nuovo il newline
            input[strcspn(input, "\n")] = 0;

            if (strcmp(input, "1") == 0) {
                create_collection(10);
                printf("A new collection is created.\n");
            } else if (strcmp(input, "2") == 0) {
                create_document("1", "content");
                printf("A new document is created.\n");
            }
        } else if (strcmp(input, "read") == 0){
            // Reading logics
        } else if (strcmp(input, "update") == 0){
            // Update logics
        } else if (strcmp(input, "remove") == 0){
            // Remove logics
        }

        if (strcmp(input, "exit") == 0){
            break; // Exiting the loop
        }
    }

    // Cleaning memory and closure
    // ...

    return 0;
}