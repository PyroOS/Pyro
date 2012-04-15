CFLAGS	+= -c
CXXFLAGS += -c
OBJDIR = objs

EXES = $(OBJDIR)/init $(OBJDIR)/ps $(OBJDIR)/ls_ports $(OBJDIR)/ls_sema	\
	$(OBJDIR)/top $(OBJDIR)/nice $(OBJDIR)/semctl $(OBJDIR)/shutdown \
	$(OBJDIR)/strace $(OBJDIR)/sysinfo mkhome kill_all

init_OBJS     = init.o
ps_OBJS       = ps.o
ls_ports_OBJS = ls_ports.o
ls_sema_OBJS  = ls_sema.o
top_OBJS      = top.o
nice_OBJS     = nice.o
semctl_OBJS   = semctl.o
shutdown_OBJS = shutdown.o
strace_OBJS   = strace.o
sysinfo_OBJS  = sysinfo.o

init_OBJS	:= $(addprefix $(OBJDIR)/,$(init_OBJS))
ps_OBJS		:= $(addprefix $(OBJDIR)/,$(ps_OBJS))
ls_ports_OBJS	:= $(addprefix $(OBJDIR)/,$(ls_ports_OBJS))
ls_sema_OBJS	:= $(addprefix $(OBJDIR)/,$(ls_sema_OBJS))
top_OBJS	:= $(addprefix $(OBJDIR)/,$(top_OBJS))
reboot_OBJS	:= $(addprefix $(OBJDIR)/,$(reboot_OBJS))
nice_OBJS	:= $(addprefix $(OBJDIR)/,$(nice_OBJS))
semctl_OBJS	:= $(addprefix $(OBJDIR)/,$(semctl_OBJS))
shutdown_OBJS	:= $(addprefix $(OBJDIR)/,$(shutdown_OBJS))
strace_OBJS	:= $(addprefix $(OBJDIR)/,$(strace_OBJS))
sysinfo_OBJS	:= $(addprefix $(OBJDIR)/,$(sysinfo_OBJS))

include $(PYRO_COMMON)/makefile.rules

all:  $(OBJDIR) $(EXES)

$(OBJDIR):
	-mkdir $(OBJDIR)

$(OBJDIR)/init: $(init_OBJS)
	$(CC) $(init_OBJS) -o $@
$(OBJDIR)/ps: $(ps_OBJS)
	$(CC) $(ps_OBJS) -o $@
$(OBJDIR)/ls_ports: $(ls_ports_OBJS)
	$(CXX) $(ls_ports_OBJS) -lpyro -o $@
$(OBJDIR)/ls_sema: $(ls_sema_OBJS)
	$(CXX) $(ls_sema_OBJS) -lpyro -o $@
$(OBJDIR)/top: $(top_OBJS)
	$(CC) $(top_OBJS) -o $@
$(OBJDIR)/nice: $(nice_OBJS)
	$(CC) $(nice_OBJS) -o $@
$(OBJDIR)/semctl: $(semctl_OBJS)
	$(CC) $(semctl_OBJS) -o $@
$(OBJDIR)/shutdown: $(shutdown_OBJS)
	$(CXX) $(shutdown_OBJS) -lpyro -o $@
$(OBJDIR)sysinfo: $(sysinfo_OBJS)
	$(CC) $(sysinfo_OBJS) -o $@
$(OBJDIR)/strace: $(strace_OBJS)
	$(CC) $(strace_OBJS) -o $@

# ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

dist: all checksum inst clean

checksum:
	for FILES in $(EXES); do \
		if test -f $$FILES; then \
			addattrib $$FILES "CheckSum::MD5" `md5sum $$FILES | cut -c1-32`; \
		fi; \
	done;
	sync

inst:
	chmod +x $(EXES)
	mkdir -p $(PYRO_BINARY_DESTINATION)/System/binary
	cp -f $(EXES) $(PYRO_BINARY_DESTINATION)/System/binary/
	ln -fs shutdown $(PYRO_BINARY_DESTINATION)/System/binary/reboot
	chmod +x $(PYRO_BINARY_DESTINATION)/System/binary/reboot
	sync

clean:
	chmod -x mkhome
	chmod -x kill_all
	rm -rf $(OBJDIR)
	sync
