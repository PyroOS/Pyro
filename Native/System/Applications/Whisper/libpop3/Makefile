CFLAGS = -O2 -Wall -c -I.

OBJS = libpop3 pop3_timer
OBJDIR = objs

OBJS := $(addprefix $(OBJDIR)/,$(addsuffix .o,$(OBJS)))

all: static shared

static:	$(OBJDIR) libpop3.a

shared: $(OBJDIR) libpop3.so

$(OBJDIR):
	@mkdir -p $@

libpop3.so: $(OBJS)
	@echo Linking : $@
	@$(CC) -shared $^ -o $@

libpop3.a: $(OBJS)
	@echo Linking : $@
	@ar rc $@ $^
	@ranlib $@

$(OBJDIR)/%.o : %.c
	@echo Compiling : $<
	@$(CC) $(CFLAGS) $< -o $@

clean:
	@if test -e libpop3.a;then rm -f libpop3.a;fi
	@if test -e libpop3.so;then rm -f libpop3.so;fi
	@if test -e pop3test;then rm -f pop3test;fi
	@rm -rf $(OBJDIR)

test: test/pop3test.c libpop3.a
	@if test -e pop3test;then rm pop3test;fi
	@$(CC) test/pop3test.c -O2 -Wall -I. -L. -lpop3 -o pop3test
