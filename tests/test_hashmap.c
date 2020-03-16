#include "test.h"
#include "hashmap.h"

START_TESTS

TEST(hashmap) {
    struct hashmap *hm = hashmap_new();
    char *value = hashmap_get(hm, "foo");
    assert_null(value);
    value = hashmap_insert(hm, "foo", "bar");
    assert_null(value);
    value = hashmap_get(hm, "foo");
    assert_str(value, "bar");
    hashmap_free(hm);
} 

TEST(dot_notation) {
    void *val;
    struct hashmap *user = hashmap_new();
    val = hashmap_insert(user, "name", "Danny");
    assert_null(val);
    struct hashmap *hm = hashmap_new();
    val = hashmap_insert(hm, "user", user);
    assert_null(val);
    assert(hashmap_resolve(hm, "user") == user, "expected user hashmap, got something else");
    val = hashmap_resolve(hm, "user.name");
    assert_str(val, "Danny");
    hashmap_free(user);
    hashmap_free(hm);
} 

TEST(hashmap_remove) {
    struct hashmap *hm = hashmap_new();
    hashmap_insert(hm, "foo", "bar");
    char *value = hashmap_get(hm, "foo");
    assert_str(value, "bar");

    // remove once
    value = hashmap_remove(hm, "foo");
    assert_str(value, "bar");
    value = hashmap_get(hm, "foo");
    assert_null(value);

    // remove again (should no-op)
    value = hashmap_remove(hm, "foo");
    assert_null(value);
    value = hashmap_get(hm, "foo");
    assert_null(value);

    hashmap_free(hm);
} 

END_TESTS