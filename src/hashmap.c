#include <string.h>
#include <stdlib.h>
#include <err.h>
#include "hashmap.h"

#define HASH(v) (v[0] - 'a') % HASHMAP_CAP

struct node {
    char *key;
    void *value;
    struct node *next;
};

struct hashmap *hashmap_new() {
    struct hashmap *hm = malloc(sizeof *hm);
    if (!hm) err(EXIT_FAILURE, "out of memory");
    for (int i=0; i < HASHMAP_CAP; i++) {
        hm->buckets[i] = NULL;
    }

    return hm;
}

void hashmap_insert(struct hashmap *hm, char *key, void *value) {
    int pos = HASH(key);
    struct node *head = hm->buckets[pos];
    struct node *node = head;

    while (node) {
        if (strcmp(node->key, key) == 0) {
            node->value = value;
            return;
        }
        node = node->next;
    }
    
    node = malloc(sizeof *node);
    node->key = key;
    node->value = value;
    node->next = head;
    hm->buckets[pos] = node;
}

void *hashmap_get(struct hashmap *hm, char *key) {
    int pos = HASH(key);
    struct node *node = hm->buckets[pos];
    while (node) {
        if (strcmp(node->key, key) == 0) {
            return node->value;
        }

        node = node->next;
    }

    return NULL;
}

void hashmap_remove(char *key) {

}

void hashmap_free(struct hashmap *hm) {
    struct node *node;
    struct node *next;

    for (int i=0; i < HASHMAP_CAP; i++) {
        node = hm->buckets[i];
        while (node) {
            next = node->next;
            free(node);            
            node = next;
        }
    }

    free(hm);
}