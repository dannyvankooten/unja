CFLAGS= -g -Wall -std=c11 -I.
LIBS= 
TESTFLAGS= $(CFLAGS) -Isrc/

all: bin/hyde

bin:; mkdir -p bin/

bin/hyde: src/hyde.c src/hashmap.c vendor/mpc.c | bin
	$(CC) $(CFLAGS) $^ -o $@

bin/test_hashmap: src/hashmap.c tests/test_hashmap.c | bin
	$(CC) $(TESTFLAGS) $^ -o $@

.PHONY: check
check: bin/test_hashmap
	for test in $^; do $$test || exit 1; done	

.PHONY: clean 
clean:; rm -r bin/