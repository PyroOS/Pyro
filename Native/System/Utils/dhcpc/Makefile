CFLAGS += -c -I.

OBJDIR = objs

OBJS = client dhcp packet interface state inbound ntp
OBJS := $(addprefix $(OBJDIR)/,$(addsuffix .o,$(OBJS)))

EXE = dhcpc

# The following debug levels are currently defined:
#
# 0 (INFO)
# 1 (ERROR)
# 2 (WARNING)
# 3 (PANIC)
#
# You can also have dhcpc dump raw (hex) packet data by defining the PKT_TRACE constant.
# Until I am sure that dhcpc is working O.K, the debug level is set to 0, without packet dumps.

CFLAGS += -DENABLE_DEBUG -DDEBUG_LEVEL=0

all: $(OBJDIR) $(OBJS)
	$(CC) $(OBJS) -o $(EXE)

$(OBJDIR):
	-mkdir $(OBJDIR)

$(OBJDIR)/%.o : %.c
	@echo Compiling : $<
	@$(CC) $(CFLAGS) $< -o $@

# ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

dist: all inst clean

inst:
	mkdir -p $(PYRO_BINARY_DESTINATION)/System/binary
	cp -f $(EXE) $(PYRO_BINARY_DESTINATION)/System/binary/
	addattrib $(PYRO_BINARY_DESTINATION)/System/binary/$(EXE) "CheckSum::MD5" `md5sum $(PYRO_BINARY_DESTINATION)/System/binary/$(EXE) | cut -c1-32`
	sync

clean:
	rm -rf $(OBJDIR)
	rm -f $(EXE)
	sync
