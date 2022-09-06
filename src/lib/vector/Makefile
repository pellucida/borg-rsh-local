
OBJ	= charvec.o pointervec.o #u8vec.o u64vec.o

CFLAGS	+= -I..
CFLAGS	+= -Wall -Os -g


ALL:	$(OBJ)

clean:
	rm -f *.o a.out

u8vec.o: ../vector/u8vec.c ../errors/errors.h ../container/container.h \
 ../container/value.h ../policy_size/policy_size.h ../vector/u8vec.h \
 ../vector/implement/u8vec.h

u64vec.o: ../vector/u64vec.c ../errors/errors.h ../container/container.h \
 ../container/value.h ../policy_size/policy_size.h ../vector/u64vec.h \
 ../vector/implement/u64vec.h

charvec.o: charvec.c ../errors/errors.h ../memory/memory.h \
 ../container/container.h ../container/value.h ../policy_size/policy_size.h \
 ../vector/charvec.h ../vector/implement/charvec.h

pointervec.o: pointervec.c ../errors/errors.h ../memory/memory.h \
 ../container/container.h ../container/value.h ../policy_size/policy_size.h \
 ../vector/pointervec.h ../vector/implement/pointervec.h
