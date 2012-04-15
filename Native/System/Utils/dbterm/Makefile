COPTS	= -c

OBJS	= term.o
EXE	= dbterm

all: dbterm

$(EXE) : $(OBJS)
	$(CC) $(OBJS) -o $(EXE)

ftp:	ftp.o
	$(CC) ftp.o -o ftp

.cpp.o :
	$(CXX) $(COPTS) $< -o $@
.c.o :
	$(CC) $(COPTS) $< -o $@

term.o : term.c

# ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

dist: all inst clean

inst:
	mkdir -p $(PYRO_BINARY_DESTINATION)/System/binary
	cp -f $(EXE) $(PYRO_BINARY_DESTINATION)/System/binary/
	addattrib $(PYRO_BINARY_DESTINATION)/System/binary/$(EXE) "CheckSum::MD5" `md5sum $(PYRO_BINARY_DESTINATION)/System/binary/$(EXE) | cut -c1-32`
	sync

clean:
	rm -f $(OBJS) dbterm rfs
	sync
