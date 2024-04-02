CC = gcc
CFLAGS = -Wall -Wextra

SRCS = gush.c histList.c 
OBJS = $(SRCS:.c=.o)

all: gush

gush: $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) gush