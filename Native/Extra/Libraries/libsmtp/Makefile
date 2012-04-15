CFLAGS := -fPIC -O2 -Wall -c -I.
LIBVER := 1

INSTALL_PATH := /usr/local
INSTALL_LIB_PATH := $(INSTALL_PATH)/lib
INSTALL_H_PATH := $(INSTALL_PATH)/include

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

install: all
	if test -e libsmtp.a; \
		then cp libsmtp.a $(INSTALL_LIB_PATH)/libsmtp_s.a; \
	fi
	if test -e libsmtp.so.$(LIBVER); \
		then cp libsmtp.so.$(LIBVER) $(INSTALL_LIB_PATH)/; \
		if test -e $(INSTALL_LIB_PATH)/libsmtp.so; \
			then rm $(INSTALL_LIB_PATH)/libsmtp.so; \
		fi; \
		ln -s $(INSTALL_LIB_PATH)/libsmtp.so.$(LIBVER) $(INSTALL_LIB_PATH)/libsmtp.so; \
		if test `uname -s` = "Linux"; \
			then /sbin/ldconfig; \
		fi; \
	fi
	cp libsmtp.h $(INSTALL_H_PATH)/

uninstall:
	-rm -f $(INSTALL_LIB_PATH)/libsmtp.so
	-rm -f $(INSTALL_LIB_PATH)/libsmtp.so.$(LIBVER)
	-rm -f $(INSTALL_LIB_PATH)/libsmtp_s.a
	-rm -f $(INSTALL_H_PATH)/libsmtp.h

clean:
	if test -e libsmtp.a;then rm libsmtp.a;fi
	if test -e libsmtp.so.*;then rm libsmtp.so.*;fi
	if test -e libsmtp.o;then rm *.o;fi
	if test -e smtptest;then rm smtptest;fi

test: smtptest.c libsmtp.a
	if test -e smtptest;then rm smtptest;fi
	$(CC) smtptest.c -Wall -O2 -I. -L. -lsmtp -o smtptest

