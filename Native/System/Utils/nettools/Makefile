COPTS	= -c -fPIC

all: route ifconfig ping

ifconfig: ifconfig.o
	$(CC) ifconfig.o -o $@

route: route.o
	$(CC) route.o -o $@

ping: ping.o
	$(CC) ping.o -o $@

.cpp.o:
	$(CXX) $(COPTS) $< -o $@

main.o : main.cpp
	$(CXX) $(COPTS) $< -o $@

# ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

dist: all inst clean

inst:
	mkdir -p $(PYRO_BINARY_DESTINATION)/System/binary
	cp -f ifconfig $(PYRO_BINARY_DESTINATION)/System/binary/
	cp -f route $(PYRO_BINARY_DESTINATION)/System/binary/
	cp -f ping $(PYRO_BINARY_DESTINATION)/System/binary/
	addattrib $(PYRO_BINARY_DESTINATION)/System/binary/ifconfig "CheckSum::MD5" `md5sum $(PYRO_BINARY_DESTINATION)/System/binary/ifconfig | cut -c1-32`
	addattrib $(PYRO_BINARY_DESTINATION)/System/binary/route "CheckSum::MD5" `md5sum $(PYRO_BINARY_DESTINATION)/System/binary/route | cut -c1-32`
	addattrib $(PYRO_BINARY_DESTINATION)/System/binary/ping "CheckSum::MD5" `md5sum $(PYRO_BINARY_DESTINATION)/System/binary/ping | cut -c1-32`
	sync

clean:
	rm -f ifconfig.o route.o ping.o
	rm -f ifconfig route ping
	sync
