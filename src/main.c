/*
 * Copyright (c) 2023, Simone Bellavia <simone.bellavia@live.it>.
 * All rights reserved.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char* id;       // Unique document ID
    char* content;  // Document contents (JSON)
} Document;

typedef struct {
    Document* documents; // Documents array
    int size;            // Number of documents in-memory
    int capacity;        // Array current capacity
} Collection;

Document* create_document(const char* id, const char* content) {
    Document* doc = malloc(sizeof(Document));
    doc->id = strdup(id);
    doc->content = strdup(content);
    return doc;
}

int main() {
    printf("Hello world!");
    return 0;
}