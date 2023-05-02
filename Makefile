CC=gcc
CFLAGS=-g -Wall -pedantic
OLIBSFLAGS=-lreadline 
LIBFLAGS=$(OLIBSFLAGS)

all: 	client \
	server \

client: client.o
	$(CC) $(CFLAGS) -o $@ client.o $(LIBFLAGS)

server: server.o function.o header.h
	$(CC) $(CFLAGS) -o $@ server.o function.o $(LIBFLAGS)


# generic
%.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o
	rm -f server
	rm -f client
	rm -f function.o
