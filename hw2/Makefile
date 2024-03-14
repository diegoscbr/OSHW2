CC = gcc
CFLAGS = -Wall -Wextra

all: simpleRead concurrent createSimpleReadProcess

simpleRead: simpleRead.c
	$(CC) $(CFLAGS) -o simpleRead simpleRead.c

concurrent: concurrent.c
	$(CC) $(CFLAGS) -o concurrent concurrent.c

createSimpleReadProcess: createSimpleReadProceess.c
	$(CC) $(CFLAGS) -o createSimpleReadProcess createSimpleReadProceess.c

.PHONY: clean
clean:
	rm -f simpleRead concurrent createSimpleReadProcess
