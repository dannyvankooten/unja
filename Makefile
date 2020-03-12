CFLAGS= -g -Wall -std=c11 -Ivendor/ -I.
LIBS= 

all: bin/hyde

bin:
	mkdir -p bin/

bin/hyde: hyde.c hashmap.c vendor/mpc.c | bin
	$(CC) $(CFLAGS) $^ -o $@

bin/test_hashmap: hashmap.c tests/test_hashmap.c | bin
	$(CC) $(CFLAGS) $^ -o $@

.PHONY: check
check: bin/test_hashmap
	for test in $^; do $$test || exit 1; done	