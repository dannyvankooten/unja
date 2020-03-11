CFLAGS= -Wall -std=c11 -Ivendor/
LIBS= 


hyde: hyde.c vendor/mpc.c
	$(CC) $(CFLAGS) $^ -o $@