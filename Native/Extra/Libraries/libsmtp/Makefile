CFLAGS := -fPIC -O2 -Wall -c -I.
LIBVER := 1

all: libsmtp.so.$(LIBVER) libsmtp.a

libsmtp.so.$(LIBVER): libsmtp.o smtp_timer.o smtp_util.o
	$(CC) -shared $^ -o $@

libsmtp.a: libsmtp.o smtp_timer.o smtp_util.o
	ar rc $@ $^
	ranlib $@

libsmtp.o: libsmtp.c libsmtp.h
	$(CC) $(CFLAGS) libsmtp.c

smtp_timer.o: smtp_timer.c smtp_timer.h
	$(CC) $(CFLAGS) smtp_timer.c

smtp_util.o: smtp_util.c libsmtp.h
	$(CC) $(CFLAGS) smtp_util.c

demoapp: libsmtp.a
	$(CC) DemoApp.c -Wall -O2 -I. -L. -lsmtp -o DemoApp

# ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

BACK=`pwd`
DESC = libsmtp 0.3 - Unfinished.

dist: all inst headers package checksum clean

inst:
	mkdir -p $(PYRO_BINARY_DESTINATION)Native/Extra/Libraries/libsmtp/System/libraries
	cp -f libsmtp.a $(PYRO_BINARY_DESTINATION)Native/Extra/Libraries/libsmtp/System/libraries/
	cp -f libsmtp.so.$(LIBVER) $(PYRO_BINARY_DESTINATION)Native/Extra/Libraries/libsmtp/System/libraries/
	ln -sf libsmtp.so$(LIBVER) $(PYRO_BINARY_DESTINATION)Native/Extra/Libraries/libsmtp/System/libraries/libsmtp.so
	addattrib $(PYRO_BINARY_DESTINATION)Native/Extra/Libraries/libsmtp/System/libraries/libsmtp.so.$(LIBVER) "os::Icon" "/System/icons/logo.png"
	addattrib $(PYRO_BINARY_DESTINATION)Native/Extra/Libraries/libsmtp/System/libraries/libsmtp.so.$(LIBVER) "os::MimeType" "pyro/library"
	sync

headers:
	mkdir -p $(PYRO_BINARY_DESTINATION)Native/Extra/Libraries/libsmtp/Include/libnet
	cp -f libsmtp.h $(PYRO_BINARY_DESTINATION)Native/Extra/Libraries/libsmtp/Include/libnet/
	addattrib $(PYRO_BINARY_DESTINATION)Native/Extra/Libraries/libsmtp/Include/libnet/libsmtp.h "os::MimeType" "text/x-h"; \
	sync

checksum:
	for FILES in `find $(PYRO_BINARY_DESTINATION)Native/Extra/Libraries/libsmtp/`; do \
		if test -f $$FILES; then \
			addattrib $$FILES "CheckSum::MD5" `md5sum $$FILES | cut -c1-32`; \
		fi; \
	done;
	sync

package:
	echo $(DESC) > $(TEMP)/zip_comment && \
	cd $(PYRO_BINARY_DESTINATION)Native/Extra/Libraries/libsmtp/ && \
	zip -z < "$(TEMP)/zip_comment" -ry9 $(PYRO_BINARY_DESTINATION)Native/Extra/Libraries/libsmtp.zip ./* && \
	cd $(BACK) && \
	rm -rf $(PYRO_BINARY_DESTINATION)Native/Extra/Libraries/libsmtp/ && \
	sync

clean:
	rm -f *.a
	rm -f *.o
	rm -f *.so.$(LIBVER)
	rm -f DemoApp
	sync
