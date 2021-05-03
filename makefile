.POSIX:

OBJS = avl.o main.o

a.out: $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJS)

clean:
	rm -f a.out $(OBJS)
