#include "test.h"
#include "hashmap.h"

START_TESTS

TEST(hashmap) {
    struct hashmap *hm = hashmap_new();
    assert(hashmap_get(hm, "foo") == NULL, "expected NULL");
    hashmap_insert(hm, "foo", "bar");
    char *value = hashmap_get(hm, "foo");
    assert_str(value, "bar");
    hashmap_free(hm);
} 

TEST(dot_notation) {
    struct hashmap *user = hashmap_new();
    hashmap_insert(user, "name", "Danny");
    struct hashmap *hm = hashmap_new();
    hashmap_insert(hm, "user", user);
    assert(hashmap_resolve(hm, "user") == user, "expected user hashmap, got something else");
    char *value = (char *) hashmap_resolve(hm, "user.name");
    assert_str(value, "Danny");
    hashmap_free(user);
    hashmap_free(hm);
} 

END_TESTS