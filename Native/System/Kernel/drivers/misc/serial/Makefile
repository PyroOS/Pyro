CFLAGS   += -kernel -fno-PIC -c
AFLAGS   = -c

C_INCLUDE_PATH=$(PYRO_INCLUDE)

OBJS := serial.o
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

all :	objs $(OBJDIR)/serial

objs:
	mkdir objs

$(OBJDIR)/serial: $(OBJS) Makefile
	$(CC) -kernel $(OBJS) -o $(OBJDIR)/serial

# ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

dist: all inst clean

inst:
	mkdir -p $(PYRO_BINARY_DESTINATION)/System/drivers/dev/misc
	cp -f $(OBJDIR)/serial $(PYRO_BINARY_DESTINATION)/System/drivers/dev/misc/
	addattrib $(PYRO_BINARY_DESTINATION)/System/drivers/dev/misc/serial "CheckSum::MD5" `md5sum $(PYRO_BINARY_DESTINATION)/System/drivers/dev/misc/serial | cut -c1-32`
	sync

clean:
	rm -rf $(OBJDIR)
	sync
