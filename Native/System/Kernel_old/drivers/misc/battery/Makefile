CFLAGS   += -kernel -fno-PIC -c -I../../bus/acpi/include
AFLAGS   = -c

C_INCLUDE_PATH=$(PYRO_INCLUDE)

OBJS := battery.o
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

all :	objs $(OBJDIR)/battery

objs:
	mkdir objs

$(OBJDIR)/battery: $(OBJS) Makefile
	$(CC) -kernel $(OBJS) -o $(OBJDIR)/battery

# ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

dist: all inst clean

inst:
	mkdir -p $(PYRO_BINARY_DESTINATION)/System/drivers/dev
	cp -f $(OBJDIR)/battery $(PYRO_BINARY_DESTINATION)/System/drivers/dev/
	addattrib $(PYRO_BINARY_DESTINATION)/System/drivers/dev/battery "CheckSum::MD5" `md5sum $(PYRO_BINARY_DESTINATION)/System/drivers/dev/battery | cut -c1-32`
	sync

clean:
	rm -rf $(OBJDIR)
	sync
