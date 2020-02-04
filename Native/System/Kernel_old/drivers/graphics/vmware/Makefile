CFLAGS   += -kernel -fno-PIC -c
AFLAGS   = -c

C_INCLUDE_PATH=$(PYRO_INCLUDE)

OBJS := driver.o
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

all :	objs $(OBJDIR)/vmware

objs:
	mkdir objs

$(OBJDIR)/vmware: $(OBJS) Makefile
	$(CC) -kernel $(OBJS) -o $(OBJDIR)/vmware

# ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

dist: all inst clean

inst:
	mkdir -p $(PYRO_BINARY_DESTINATION)/System/drivers/dev/graphics
	cp -f $(OBJDIR)/vmware $(PYRO_BINARY_DESTINATION)/System/drivers/dev/graphics/
	addattrib $(PYRO_BINARY_DESTINATION)/System/drivers/dev/graphics/vmware "CheckSum::MD5" `md5sum $(PYRO_BINARY_DESTINATION)/System/drivers/dev/graphics/vmware | cut -c1-32`
	sync

clean:
	rm -rf $(OBJDIR)
	sync
