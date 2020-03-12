CFLAGS= -g -Wall -std=c11 -I.
LIBS= 
TESTFLAGS= $(CFLAGS) -Isrc/

all: bin/hyde

bin:; mkdir -p bin/

bin/hyde: src/hyde.c src/hashmap.c src/template.c src/vector.c vendor/mpc.c | bin
	$(CC) $(CFLAGS) $^ -o $@

bin/test_hashmap: src/hashmap.c tests/test_hashmap.c | bin
	$(CC) $(TESTFLAGS) $^ -o $@

bin/test_template: src/template.c src/hashmap.c src/vector.c tests/test_template.c vendor/mpc.c | bin 
	$(CC) $(TESTFLAGS) $^ -o $@

.PHONY: check
check: bin/test_hashmap bin/test_template
	for test in $^; do $$test || exit 1; done	

.PHONY: clean 
clean:; rm -r bin/