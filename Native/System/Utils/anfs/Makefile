COPTS	= -c

OBJS	= server.o
EXE	= anfs-server

$(EXE) : $(OBJS)
	$(CC) $(OBJS) -o $@

.cpp.o:
	$(CC) $(COPTS) $< -o $@

server.o: server.c
	$(CC) $(COPTS) $< -o $@

# ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

dist: $(EXE) inst clean

inst:
	mkdir -p $(PYRO_BINARY_DESTINATION)/System/binary
	cp -f $(EXE) $(PYRO_BINARY_DESTINATION)/System/binary/
	addattrib $(PYRO_BINARY_DESTINATION)/System/binary/$(EXE) "CheckSum::MD5" `md5sum $(PYRO_BINARY_DESTINATION)/System/binary/$(EXE) | cut -c1-32`
	sync

clean:
	rm -f $(OBJS)
	rm -f anfs-server
	sync
