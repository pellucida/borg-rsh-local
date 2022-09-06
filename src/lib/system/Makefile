

OBJ	= process.o passwd.o

CFLAGS	+= -I.. -Os -g
CFLAGS	+= -Wall


ALL:	$(OBJ)

passwd.o: ../system/passwd.c ../errors/errors.h ../string/str.h ../string/strvec.h \
 ../system/passwd.h ../memory/memory.h

process.o: ../system/process.c ../errors/errors.h ../string/str.h \
 ../string/strvec.h ../system/process.h 

clean:
	rm -f *.o a.out
