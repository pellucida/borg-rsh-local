

OBJ	= str.o strvec.o

CFLAGS	+= -I.. -Os -g
CFLAGS	+= -Wall


ALL:	$(OBJ)

clean:
	rm -f *.o a.out

str.o: ../string/str.c ../vector/charvec.h ../errors/errors.h ../string/str.h

strvec.o: ../string/strvec.c ../errors/errors.h ../container/container.h \
 ../vector/pointervec.h \
 ../string/str.h ../string/strvec.h
