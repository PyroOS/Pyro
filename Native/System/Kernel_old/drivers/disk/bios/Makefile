CFLAGS   += -kernel -D__BDD_EXTERNAL -fno-PIC -c
AFLAGS   = -c

C_INCLUDE_PATH=$(PYRO_INCLUDE)

OBJS := bdd.o
OBJDIR	:= objs
OBJS	:= $(addprefix $(OBJDIR)/,$(OBJS))

#
#	rules
#

$(OBJDIR)/%.o : %.c
	$(CC) $(CFLAGS) $< -o $@

$(OBJDIR)/%.o : %.cpp
	$(CXX) $(CFLAGS) $< -o $@

$(OBJDIR)/%.o : %.s
	$(CC) $(AFLAGS) -x assembler-with-cpp $< -o $@

$(OBJDIR)/%.o : %.S
	$(CC) $(CFLAGS) -x assembler-with-cpp $< -o $@

all :	objs $(OBJDIR)/bios

objs:
	mkdir objs

$(OBJDIR)/bios: $(OBJS) Makefile
	$(CC) -kernel $(OBJS) -o $(OBJDIR)/bios

# ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

dist: all inst clean

inst:
	mkdir -p $(PYRO_BINARY_DESTINATION)/System/drivers/dev/disk
	cp -f $(OBJDIR)/bios $(PYRO_BINARY_DESTINATION)/System/drivers/dev/disk/
	addattrib $(PYRO_BINARY_DESTINATION)/System/drivers/dev/disk/bios "CheckSum::MD5" `md5sum $(PYRO_BINARY_DESTINATION)/System/drivers/dev/disk/bios | cut -c1-32`
	sync

clean:
	rm -rf $(OBJDIR)
	sync
