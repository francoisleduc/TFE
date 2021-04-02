# COMPILER #
CC=gcc
LD=gcc

# FLAGS #
CFLAGS=-Wall -Wmissing-prototypes
LDFLAGS=-lpthread

# CONFIGURATION FILE #
EXEC=switch

# COMMANDS #
all: $(EXEC)
$(EXEC): client.o log.o linkedlist.o common.o
client.o: client.c client.h
	$(CC) -c client.c -o client.o $(CFLAGS)
linkedlist.o: linkedlist.c linkedlist.h
	$(CC) -c linkedlist.c -o linkedlist.o $(CFLAGS)
log.o: log.c log.h
	$(CC) -c log.c -o log.o $(CFLAGS)
common.o: common.c common.h
	$(CC) -c common.c -o common.o $(CFLAGS)
clean:
	rm -rf *.o
proper:
	rm switch
