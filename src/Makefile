
BORGRSH		= borg-rsh-local


LIBSRC	= lib
OPTFLAG	= -O3
CFLAGS	+= -g -I$(LIBSRC)
CFLAGS	+= $(OPTFLAG)
OBJ	= $(LIBSRC)/string/strvec.o $(LIBSRC)/string/str.o
OBJ	+= $(LIBSRC)/system/passwd.o $(LIBSRC)/system/process.o
OBJ	+= $(LIBSRC)/vector/charvec.o $(LIBSRC)/vector/pointervec.o
OBJ	+= $(LIBSRC)/memory/memory.o

BORGRSH_OBJ	= $(BORGRSH).o

$(BORGRSH):	$(BORGRSH_OBJ) $(OBJ)
	$(CC) -o $@ $(CFLAGS) $(BORGRSH_OBJ) $(OBJ)

distclean: clean
	rm -f $(BORGRSH)

clean:
	rm -f $(OBJ)
	rm -f $(BORGRSH_OBJ) a.out

# Override with own parameters
# make -e BORG_USER=testborg -e BORG_REPOPATH=/var/tmp/borg -e BORG_REPO=testing test
#
BORG_USER=backup
BORG_REPOPATH=/borg/repo
BORG_REPO=example
test:
	borg list --rsh=./$(BORGRSH) $(BORG_USER)@localhost:$(BORG_REPOPATH)::$(BORG_REPO)

borg-rsh-local.o: borg-rsh-local.c $(LIBSRC)/errors/errors.h $(LIBSRC)/string/str.h \
 $(LIBSRC)/errors/errors.h $(LIBSRC)/string/strvec.h $(LIBSRC)/system/passwd.h $(LIBSRC)/system/process.h \
 $(LIBSRC)/string/str.h 
