#include "unity.h"
#include "../src/db_manager.h"

void setUp(void) {
    // empty
}

void tearDown(void) {
    // empty
}

/* Verify that the HashTable isn't NULL and that
all its initial properties are being set up correctly */
void test_create_hash_table(void) {
    HashTable *table = create_hash_table();
    TEST_ASSERT_NOT_NULL(table);
    TEST_ASSERT_NOT_NULL(table->buckets);
    TEST_ASSERT_EQUAL_INT(INITIAL_HASH_TABLE_SIZE, table->size);
    
    free_hash_table(table);
}

/* Verify that the function is returning a consistent
value for the same input str */
void test_hash_function(void) {
    char *key = "testKey";
    unsigned long hash1 = hash_function(key);
    unsigned long hash2 = hash_function(key);
    TEST_ASSERT_EQUAL_UINT64(hash1, hash2);
}

void test_hash_function_different_input(void) {
    char *key1 = "testKey1";
    char *key2 = "testKey2";
    unsigned long hash1 = hash_function(key1);
    unsigned long hash2 = hash_function(key2);
    TEST_ASSERT_NOT_EQUAL(hash1, hash2);
}

/* Verify that a new HashEntry is being created 
and that its fields are populated with provided values */
void test_create_hash_entry(void) {
    char *key = "testKey";
    unsigned long hash = hash_function(key);
    Document *doc = create_document("testContent");
    HashEntry *entry = create_hash_entry(key, hash, doc);

    TEST_ASSERT_NOT_NULL(entry);
    TEST_ASSERT_EQUAL_STRING(key, entry->key);
    TEST_ASSERT_EQUAL_UINT64(hash, entry->hash);
    TEST_ASSERT_EQUAL_PTR(doc, entry->value);

    free_hash_entry(entry);
}

void test_create_hash_entry_collision_handling(void) {
    char *key1 = "testKey";
    char *key2 = "testKey";
    unsigned long hash1 = hash_function(key1);
    unsigned long hash2 = hash_function(key2);

    Document *doc1 = create_document("Content1");
    Document *doc2 = create_document("Content2");

    HashEntry *entry1 = create_hash_entry(key1, hash1, doc1);
    HashEntry *entry2 = create_hash_entry(key2, hash2, doc2);

    free_hash_entry(entry1);
    free_hash_entry(entry2);
}

void test_create_hash_entry_edge_cases(void) {
    char *key = "testKey";
    unsigned long hash = hash_function(key);

    HashEntry *entry_with_null_doc = create_hash_entry(key, hash, NULL);
    TEST_ASSERT_NOT_NULL(entry_with_null_doc);
    TEST_ASSERT_EQUAL_STRING(key, entry_with_null_doc->key);
    TEST_ASSERT_EQUAL_UINT64(hash, entry_with_null_doc->hash);
    TEST_ASSERT_NULL(entry_with_null_doc->value);
    free_hash_entry(entry_with_null_doc);
}

void test_create_collection(void) {
    Collection *collection = create_collection();
    TEST_ASSERT_NOT_NULL(collection);
    TEST_ASSERT_EQUAL_INT(0, collection->size);
    TEST_ASSERT_NULL(collection->documents);

    free_collection(collection);
}

void test_generate_unique_id(void) {
    char *id1 = generate_unique_id();
    char *id2 = generate_unique_id();

    TEST_ASSERT_NOT_NULL(id1);
    TEST_ASSERT_NOT_NULL(id2);
    TEST_ASSERT_NOT_EQUAL_MESSAGE(id1, id2, "Generated IDs should be different");

    free(id1);
    free(id2);
}

void test_create_document(void) {
    char *content = "testContent";
    Document *doc = create_document(content);

    TEST_ASSERT_NOT_NULL(doc);
    TEST_ASSERT_NOT_NULL(doc->id);
    TEST_ASSERT_EQUAL_STRING(content, doc->content);

    free_document(doc);
}

void test_create_hash_table_should_allocate_table_with_null_buckets(void) {
    HashTable *table = create_hash_table();
    TEST_ASSERT_NOT_NULL(table);
    TEST_ASSERT_EQUAL(INITIAL_HASH_TABLE_SIZE, table->size);

    for (int i = 0; i < INITIAL_HASH_TABLE_SIZE; i++) {
        TEST_ASSERT_NULL(table->buckets[i]);
    }

    free(table->buckets);
    free(table);
}

void test_create_collection_should_initialize_properly(void) {
    Collection *collection = create_collection();
    TEST_ASSERT_NOT_NULL(collection);
    TEST_ASSERT_NULL(collection->documents);
    TEST_ASSERT_EQUAL(0, collection->size);
    TEST_ASSERT_EQUAL(0, collection->capacity);
    TEST_ASSERT_NOT_NULL(collection->hashTable);

    TEST_ASSERT_EQUAL(INITIAL_HASH_TABLE_SIZE, collection->hashTable->size);
    for (int i = 0; i < collection->hashTable->size; i++) {
        TEST_ASSERT_NULL(collection->hashTable->buckets[i]);
    }

    free(collection->hashTable->buckets);
    free(collection->hashTable);
    free(collection);
}

int main(void){
    printf("Starting tests...\n");
    UNITY_BEGIN();
    RUN_TEST(test_create_hash_table_should_allocate_table_with_null_buckets);
    RUN_TEST(test_create_collection_should_initialize_properly);

    RUN_TEST(test_create_hash_table);
    RUN_TEST(test_hash_function);
    RUN_TEST(test_create_hash_entry);
    RUN_TEST(test_create_collection);
    RUN_TEST(test_generate_unique_id);
    RUN_TEST(test_create_document);
    RUN_TEST(test_hash_function_different_input);
    RUN_TEST(test_create_hash_entry_collision_handling);
    RUN_TEST(test_create_hash_entry_edge_cases);

    printf("Tests completed...\n");
    return UNITY_END();
}