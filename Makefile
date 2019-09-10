OBJS=valkyrie.o

all:$(OBJS)
	$(CC) -o valkyrie $(OBJS)


clean:
	-rm valkyrie 
	-rm valkyrie.o
