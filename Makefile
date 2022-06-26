CC=gcc
CFLAGS=-I.

client: client.c requests.c helpers.c buffer.c parson.c Utils.c
	$(CC) -o client client.c requests.c helpers.c buffer.c parson.c Utils.c

run: client
	./client

clean:
	rm -f *.o client
