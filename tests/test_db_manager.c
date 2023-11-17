#include "unity.h"
#include "../src/db_manager.h"

void setUp(void) {
    // empty
}

void tearDown(void) {
    // empty
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
    printf("Tests completed...\n");
    return UNITY_END();
}