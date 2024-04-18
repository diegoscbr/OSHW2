CC = gcc
CFLAGS = -Wall -Wextra

SRCS = pageExtraction.c tlbQueue.c 
OBJS = $(SRCS:.c=.o)

all: memSim

memSim: $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) a.out