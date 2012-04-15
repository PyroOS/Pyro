CFLAGS   += -kernel -fno-PIC -c
AFLAGS   = -c

C_INCLUDE_PATH=$(PYRO_INCLUDE)

OBJS := ps2.o
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

all :	objs $(OBJDIR)/ps2

objs:
	mkdir objs

$(OBJDIR)/ps2: $(OBJS) Makefile
	$(CC) -kernel $(OBJS) -o $(OBJDIR)/ps2

# ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

dist: all inst clean

inst:
	mkdir -p $(PYRO_BINARY_DESTINATION)/System/drivers/dev
	cp -f $(OBJDIR)/ps2 $(PYRO_BINARY_DESTINATION)/System/drivers/dev/
	addattrib $(PYRO_BINARY_DESTINATION)/System/drivers/dev/ps2 "CheckSum::MD5" `md5sum $(PYRO_BINARY_DESTINATION)/System/drivers/dev/ps2 | cut -c1-32`
	sync

clean:
	rm -rf $(OBJDIR)
	sync
