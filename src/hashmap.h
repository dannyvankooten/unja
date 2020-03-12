#define HASHMAP_CAP 26

struct hashmap {
    struct node *buckets[HASHMAP_CAP];
};

void hashmap_insert(struct hashmap *hm, char *key, void *value);
void *hashmap_get(struct hashmap *hm, char *key);
void hashmap_remove(char *key);
struct hashmap *hashmap_new();
void hashmap_free(struct hashmap *hm);