

OBJ	= process.o passwd.o readlink.o realpath.o stat.o procfs_pid.o
CFLAGS	+= -I.. -Os -g
CFLAGS	+= -Wall


ALL:	$(OBJ)
clean:
	rm -f *.o a.out

passwd.o: ../system/passwd.c ../errors/errors.h ../string/str.h ../string/strvec.h \
 ../system/passwd.h ../memory/memory.h

process.o: ../system/process.c ../errors/errors.h ../string/str.h \
 ../string/strvec.h ../system/process.h 

readlink.o: readlink.c ../errors/errors.h ../string/str.h \
 ../system/stat.h ../system/readlink.h

realpath.o: realpath.c ../errors/errors.h ../string/str.h \
 ../system/realpath.h

stat.o: stat.c ../errors/errors.h ../string/str.h ../system/stat.h

test.o: test.c ../errors/errors.h ../string/str.h ../string/strvec.h \
 ../system/stat.h ../system/readlink.h ../system/realpath.h
