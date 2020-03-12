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


TEST(dot_notation) {

    struct hashmap *user = hashmap_new();
    hashmap_insert(user, "name", "Danny");
    struct hashmap *hm = hashmap_new();
    hashmap_insert(hm, "user", user);
    char *value = (char *) hashmap_resolve(hm, "user.name");
    assert(value != NULL, "expected value, got NULL");
    assert(strcmp(value, "Danny") == 0, "expected %s, got %s", "Danny", value);

    hashmap_free(hm);
} 

END_TESTS