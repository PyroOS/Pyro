CFLAGS = -O3 -c

OBJS = main.o

ctail: $(OBJS)
	g++ $(OBJS) -o ctail

.cpp.o:
	g++ $(CFLAGS) $< -o $@

main.o: main.cpp

# ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

dist: ctail inst web clean

inst:
	mkdir -p $(PYRO_BINARY_DESTINATION)Native/Development/System/binary
	cp -f ctail $(PYRO_BINARY_DESTINATION)Native/Development/System/binary/
	addattrib $(PYRO_BINARY_DESTINATION)Native/Development/System/binary/ctail "CheckSum::MD5" `md5sum $(PYRO_BINARY_DESTINATION)Native/Development/System/binary/ctail | cut -c1-32`
	sync

web:
	mkdir -p $(PYRO_BINARY_DESTINATION)Web/Languages/LIST
	mkdir -p $(PYRO_BINARY_DESTINATION)Web/Languages/Development/ctail
	sync

clean:
	rm -f ctail main.o
	sync
