CFLAGS := -fPIC -O2 -Wall -c -I.
LIBVER := 1

all: libpop3.so.$(LIBVER) libpop3.a

libpop3.so.$(LIBVER): libpop3.o pop3_timer.o
	$(CC) -shared $^ -o $@

libpop3.a: libpop3.o pop3_timer.o
	ar rc $@ $^
	ranlib $@

libpop3.o: libpop3.c libpop3.h
	$(CC) $(CFLAGS) libpop3.c

pop3_timer.o: pop3_timer.c pop3_timer.h
	$(CC) $(CFLAGS) pop3_timer.c

demoapp: libpop3.a
	$(CC) DemoApp.c -Wall -O2 -I. -L. -lpop3 -o DemoApp

# ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

BACK=`pwd`
DESC = libpop3 0.5 - Unfinished.

dist: all inst headers package checksum clean

inst:
	mkdir -p $(PYRO_BINARY_DESTINATION)Native/Extra/Libraries/libpop3/System/libraries
	cp -f libpop3.a $(PYRO_BINARY_DESTINATION)Native/Extra/Libraries/libpop3/System/libraries/
	cp -f libpop3.so.$(LIBVER) $(PYRO_BINARY_DESTINATION)Native/Extra/Libraries/libpop3/System/libraries/
	ln -sf libpop3.so$(LIBVER) $(PYRO_BINARY_DESTINATION)Native/Extra/Libraries/libpop3/System/libraries/libpop3.so
	addattrib $(PYRO_BINARY_DESTINATION)Native/Extra/Libraries/libpop3/System/libraries/libpop3.so.$(LIBVER) "os::Icon" "/System/icons/logo.png"
	addattrib $(PYRO_BINARY_DESTINATION)Native/Extra/Libraries/libpop3/System/libraries/libpop3.so.$(LIBVER) "os::MimeType" "pyro/library"
	sync

headers:
	mkdir -p $(PYRO_BINARY_DESTINATION)Native/Extra/Libraries/libpop3/Include/libnet
	cp -f libpop3.h $(PYRO_BINARY_DESTINATION)Native/Extra/Libraries/libpop3/Include/libnet/
	addattrib $(PYRO_BINARY_DESTINATION)Native/Extra/Libraries/libpop3/Include/libnet/libpop3.h "os::MimeType" "text/x-h"; \
	sync

checksum:
	for FILES in `find $(PYRO_BINARY_DESTINATION)Native/Extra/Libraries/libpop3/`; do \
		if test -f $$FILES; then \
			addattrib $$FILES "CheckSum::MD5" `md5sum $$FILES | cut -c1-32`; \
		fi; \
	done;
	sync

package:
	echo $(DESC) > $(TEMP)/zip_comment && \
	cd $(PYRO_BINARY_DESTINATION)Native/Extra/Libraries/libpop3/ && \
	zip -z < "$(TEMP)/zip_comment" -ry9 $(PYRO_BINARY_DESTINATION)Native/Extra/Libraries/libpop3.zip ./* && \
	cd $(BACK) && \
	rm -rf $(PYRO_BINARY_DESTINATION)Native/Extra/Libraries/libpop3/ && \
	sync

clean:
	rm -f *.a
	rm -f *.o
	rm -f *.so.$(LIBVER)
	rm -f DemoApp
	sync
