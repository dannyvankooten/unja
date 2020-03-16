#define HASHMAP_CAP 26

struct hashmap {
    struct node *buckets[HASHMAP_CAP];
};

struct hashmap *hashmap_new();
void *hashmap_insert(struct hashmap *hm, char *key, void *value);
void *hashmap_get(struct hashmap *hm, char *key);
void *hashmap_resolve(struct hashmap *hm, char *key);
void *hashmap_remove(struct hashmap *hm, char *key);
void hashmap_free(struct hashmap *hm);