

OBJ	= memory.o

CFLAGS	+= -I.. -Os -g
CFLAGS	+= -Wall


ALL:	$(OBJ)

clean:
	rm -f *.o a.out

memory.o: memory.c ../errors/errors.h ../memory/memory.h
