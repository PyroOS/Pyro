CFLAGS   += -kernel -fno-PIC -c
AFLAGS   = -c

C_INCLUDE_PATH=$(PYRO_INCLUDE)

OBJS := devices.o
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

all :	objs $(OBJDIR)/apm

objs:
	mkdir objs

$(OBJDIR)/apm: $(OBJS) Makefile
	$(CC) -kernel $(OBJS) -o $(OBJDIR)/apm

# ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

dist: all inst clean

inst:
	mkdir -p $(PYRO_BINARY_DESTINATION)/System/drivers/dev
	cp -f $(OBJDIR)/apm $(PYRO_BINARY_DESTINATION)/System/drivers/dev/
	addattrib $(PYRO_BINARY_DESTINATION)/System/drivers/dev/apm "CheckSum::MD5" `md5sum $(PYRO_BINARY_DESTINATION)/System/drivers/dev/apm | cut -c1-32`
	sync

clean:
	rm -rf $(OBJDIR)
	sync
