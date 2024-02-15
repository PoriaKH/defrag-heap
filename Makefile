SRCS=mm_alloc/mm_alloc.c mm_test/mm_test.c
EXECUTABLES=allocator

CC=gcc
CFLAGS=-g -Wall
LDFLAGS=

OBJS=$(SRCS:.c=.o)

all: $(EXECUTABLES) run

run: allocator
	./allocator

$(EXECUTABLES): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LDFLAGS) -o $@  

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(EXECUTABLES) $(OBJS)

