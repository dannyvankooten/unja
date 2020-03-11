CFLAGS= -g -Wall -std=c11 -Ivendor/
LIBS= 

hyde: hyde.c hashmap.c vendor/mpc.c
	$(CC) $(CFLAGS) $^ -o $@