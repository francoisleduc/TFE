# COMPILER #
CC=gcc
CPP=g++
LD=gcc
AR=ar

# FLAGS #
CFLAGS=-Wall -Wextra
LDFLAGS=-lpthread

# CONFIGURATION FILE #
EXEC=switch
EXEC2=lambda_server
RLEASE=release

# COMMANDS #
all: $(EXEC) $(EXEC2) $(RLEASE)
$(EXEC): fake_switch.o client.o log.o linkedlist.o
	$(LD) -o ../netsimulation/$(EXEC) fake_switch.o client.o linkedlist.o log.o $(LDFLAGS)
$(EXEC2): server.o lambda_server.o
	$(CPP) -o ../netsimulation/$(EXEC2) server.o lambda_server.o $(LDFLAGS)
	$(MAKE) $(RLEASE)

client.o: client.c client.h common.h
	$(CC) -c client.c -o client.o $(CFLAGS)
linkedlist.o: linkedlist.c linkedlist.h
	$(CC) -c linkedlist.c -o linkedlist.o $(CFLAGS)
log.o: log.c log.h
	$(CC) -c log.c -o log.o $(CFLAGS)
fake_switch.o: fake_switch.c
	$(CC) -c fake_switch.c -o fake_switch.o $(CFLAGS)


server.o: Server.cpp Server.h
	$(CPP) -c Server.cpp -o server.o $(CFLAGS) -std=c++14
lambda_server.o: lambda.cpp
	$(CPP) -c lambda.cpp -o lambda_server.o $(CFLAGS) -std=c++14

release:
	cp -f ../netsimulation/lambda_server ../../XDP-Code/netsimulation
archive:
	tar -zcvf $(EXEC).tar.gz Makefile
clean:
	rm -rf *.o
proper:
	rm ../netsimulation/lambda_server ../netsimulation/switch ../../XDP-Code/netsimulation/lambda_server
push:
	cp -f log.c ../../XDP-Code/protocol
	cp -f log.h ../../XDP-Code/protocol
	cp -f linkedlist.c ../../XDP-Code/protocol
	cp -f linkedlist.h ../../XDP-Code/protocol
	cp -f client.c ../../XDP-Code/protocol
	cp -f client.h ../../XDP-Code/protocol
	cp -f common.h ../../XDP-Code/protocol

