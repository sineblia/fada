/*
 * Copyright (c) 2023, Simone Bellavia <simone.bellavia@live.it>.
 * All rights reserved.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Basic data structures
typedef struct {
    char* id;       // Unique document ID
    char* content;  // Document contents (JSON)
} Document;

typedef struct {
    Document* documents; // Documents array
    int size;            // Number of documents in-memory
    int capacity;        // Array current capacity
} Collection;

// CRUD
Document* create_document(const char* id, const char* content) {
    Document* doc = malloc(sizeof(Document));
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

void read_document(const char* id);
void update_document(const char* id, const char* content);
void delete_document(const char* id);

int main() {
    printf("Hello world!");
    return 0;
}