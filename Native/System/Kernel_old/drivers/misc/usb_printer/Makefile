CFLAGS   += -D__ENABLE_DEBUG__ -kernel -fno-PIC -c --freestanding
AFLAGS   = -c

C_INCLUDE_PATH=$(PYRO_INCLUDE)

OBJS := usbprinter.o
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

all :	objs $(OBJDIR)/usb_printer

objs:
	mkdir objs

$(OBJDIR)/usb_printer: $(OBJS) Makefile
	$(CC) -kernel $(OBJS) -o $(OBJDIR)/usb_printer

# ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

dist: all inst clean

inst:
	mkdir -p $(PYRO_BINARY_DESTINATION)/System/drivers/dev/printer
	cp -f $(OBJDIR)/usb_printer $(PYRO_BINARY_DESTINATION)/System/drivers/dev/printer/
	addattrib $(PYRO_BINARY_DESTINATION)/System/drivers/dev/printer/usb_printer "CheckSum::MD5" `md5sum $(PYRO_BINARY_DESTINATION)/System/drivers/dev/printer/usb_printer | cut -c1-32`
	sync

clean:
	rm -rf $(OBJDIR)
	sync
