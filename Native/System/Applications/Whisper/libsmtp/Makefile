CFLAGS = -O2 -Wall -c -I.

OBJS = libsmtp smtp_timer smtp_util smtp_auth base64 MD5
OBJDIR = objs

OBJS := $(addprefix $(OBJDIR)/,$(addsuffix .o,$(OBJS)))

all: static shared

static:	$(OBJDIR) libsmtp.a

shared: $(OBJDIR) libsmtp.so

$(OBJDIR):
	@mkdir -p $@

libsmtp.so: $(OBJS)
	@echo Linking : $@
	@$(CC) -shared $^ -o $@

libsmtp.a: $(OBJS)
	@echo Linking : $@
	@ar rc $@ $^
	@ranlib $@

$(OBJDIR)/%.o : %.c
	@echo Compiling : $<
	@$(CC) $(CFLAGS) $< -o $@

clean:
	@if test -e libsmtp.a;then rm libsmtp.a;fi
	@if test -e libsmtp.so;then rm libsmtp.so;fi
	@if test -e smtptest;then rm smtptest;fi
	@-rm $(OBJDIR)/*

test: test/smtptest.c libsmtp.a
	@if test -e smtptest;then rm smtptest;fi
	@$(CC) test/smtptest.c -O2 -Wall -I. -L. -lsmtp -o smtptest

