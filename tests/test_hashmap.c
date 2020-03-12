#include "test.h"
#include "hashmap.h"

START_TESTS

TEST(hashmap) {
    struct hashmap *hm = hashmap_new();
    assert(hashmap_get(hm, "foo") == NULL, "expected NULL");

    hashmap_insert(hm, "foo", "bar");
    char *value = hashmap_get(hm, "foo");
    assert(value != NULL, "expected value, got NULL");
    assert(strcmp(value, "bar") == 0, "expected %s, got %s", "bar", value);

    hashmap_free(hm);
} 

END_TESTS