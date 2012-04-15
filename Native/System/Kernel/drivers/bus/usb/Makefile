CFLAGS   += -kernel -fno-PIC -c -D__ENABLE_DEBUG__
AFLAGS   = -c

C_INCLUDE_PATH=$(PYRO_INCLUDE)

OBJS := usb_hub.o usb.o libusb.o

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

all :	objs $(OBJDIR)/usb

objs:
	mkdir objs

$(OBJDIR)/usb: $(OBJS) Makefile
	$(CC) -kernel $(OBJS) -o $(OBJDIR)/usb

# ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

dist: all inst clean

inst:
	mkdir -p $(PYRO_BINARY_DESTINATION)/System/drivers/dev/bus
	cp -f $(OBJDIR)/usb $(PYRO_BINARY_DESTINATION)/System/drivers/dev/bus/
	addattrib $(PYRO_BINARY_DESTINATION)/System/drivers/dev/bus/usb "CheckSum::MD5" `md5sum $(PYRO_BINARY_DESTINATION)/System/drivers/dev/bus/usb | cut -c1-32`
	sync

clean:
	rm -rf $(OBJDIR)
	sync
