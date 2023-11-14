/*
 * Copyright (c) 2023, Simone Bellavia <simone.bellavia@live.it>.
 * All rights reserved.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

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

/* CRUD functions */
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

char* read_document(const char* id){

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

bool update_document(const char* id, const char* new_content){
    char filename[256];
    snprintf(filename, sizeof(filename), "%s.json", id);

    FILE *file = fopen(filename, "w");
    if (file == NULL){
        return false;
    }

    fprintf(file, "%s", new_content);
    fclose(file);

    return true;
}

void delete_document(const char* id);

int main() {
    // char* id = "testdoc";
    // char* content = "{ \"name\": \"test\" }";

    // Document* doc = create_document(id, content);

    return 0;
}