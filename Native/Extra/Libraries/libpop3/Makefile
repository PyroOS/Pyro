CFLAGS := -fPIC -O2 -Wall -c -I.
LIBVER := 1

INSTALL_PATH := /usr/local
INSTALL_LIB_PATH := $(INSTALL_PATH)/lib
INSTALL_H_PATH := $(INSTALL_PATH)/include

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

install: all
	if test -e libpop3.a; \
		then cp libpop3.a $(INSTALL_LIB_PATH)/libpop3_s.a; \
	fi
	if test -e libpop3.so.$(LIBVER); \
		then cp libpop3.so.$(LIBVER) $(INSTALL_LIB_PATH)/; \
		if test -e $(INSTALL_LIB_PATH)/libpop3.so; \
			then rm $(INSTALL_LIB_PATH)/libpop3.so; \
		fi; \
		ln -s $(INSTALL_LIB_PATH)/libpop3.so.$(LIBVER) $(INSTALL_LIB_PATH)/libpop3.so; \
		if test `uname -s` = "Linux"; \
			then /sbin/ldconfig; \
		fi; \
	fi
	cp libpop3.h $(INSTALL_H_PATH)/

uninstall:
	-rm -f $(INSTALL_LIB_PATH)/libpop3.so
	-rm -f $(INSTALL_LIB_PATH)/libpop3.so.$(LIBVER)
	-rm -f $(INSTALL_LIB_PATH)/libpop3_s.a
	-rm -f $(INSTALL_H_PATH)/libpop3.h

clean:
	if test -e libpop3.a;then rm libpop3.a;fi
	if test -e libpop3.so*.s;then rm libpop3.so.*;fi
	if test -e libpop3.o;then rm *.o;fi
	if test -e pop3test;then rm pop3test;fi

test: pop3test.c libpop3.a
	if test -e pop3test;then rm pop3test;fi
	$(CC) pop3test.c -Wall -O2 -I. -L. -lpop3 -o pop3test
