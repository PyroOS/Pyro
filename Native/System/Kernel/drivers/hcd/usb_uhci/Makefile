CFLAGS   += -kernel -fno-PIC -c -D__ENABLE_DEBUG__
AFLAGS   = -c

C_INCLUDE_PATH=$(PYRO_INCLUDE)

OBJS := usb-uhci.o
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

all :	objs $(OBJDIR)/usb_uhci

objs:
	mkdir objs

$(OBJDIR)/usb_uhci: $(OBJS) Makefile
	$(CC) -kernel $(OBJS) -o $(OBJDIR)/usb_uhci

# ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

dist: all inst clean

inst:
	mkdir -p $(PYRO_BINARY_DESTINATION)/System/drivers/dev/hcd
	cp -f $(OBJDIR)/usb_uhci $(PYRO_BINARY_DESTINATION)/System/drivers/dev/hcd/
	addattrib $(PYRO_BINARY_DESTINATION)/System/drivers/dev/hcd/usb_uhci "CheckSum::MD5" `md5sum $(PYRO_BINARY_DESTINATION)/System/drivers/dev/hcd/usb_uhci | cut -c1-32`
	sync

clean:
	rm -rf $(OBJDIR)
	sync
