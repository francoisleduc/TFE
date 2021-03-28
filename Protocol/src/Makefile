# COMPILER #
CC=gcc
LD=gcc
AR=ar

# FLAGS #
CFLAGS=-Wall -Wmissing-prototypes -luuid
LDFLAGS=-luuid -lpthread

# CONFIGURATION FILE #
EXEC=switch
EXEC2=lambda_server


# COMMANDS #
all: $(EXEC) $(EXEC2)
$(EXEC): fake_switch.o client.o log.o linkedlist.o common.o
	$(LD) -o ../netsimulation/$(EXEC) fake_switch.o client.o linkedlist.o log.o common.o $(LDFLAGS)
$(EXEC2): server.o log.o linkedlist.o common.o
	$(LD) -o ../netsimulation/$(EXEC2) server.o linkedlist.o log.o common.o $(LDFLAGS)
client.o: client.c client.h
	$(CC) -c client.c -o client.o $(CFLAGS)
linkedlist.o: linkedlist.c linkedlist.h
	$(CC) -c linkedlist.c -o linkedlist.o $(CFLAGS)
log.o: log.c log.h
	$(CC) -c log.c -o log.o $(CFLAGS)
server.o: server.c
	$(CC) -c server.c -o server.o $(CFLAGS)
common.o: common.c common.h
	$(CC) -c common.c -o common.o $(CFLAGS)
fake_switch.o: fake_switch.c
	$(CC) -c fake_switch.c -o fake_switch.o $(CFLAGS)

archive:
	tar -zcvf $(EXEC).tar.gz Makefile
clean:
	rm -rf *.o
proper:
	rm ../netsimulation/lambda_server ../netsimulation/switch
