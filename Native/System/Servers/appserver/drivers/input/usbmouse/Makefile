COPTS   = -I../../../server -c

OBJS =	usbmouse.o
OBJDIR	:= objs
OBJS	:= $(addprefix $(OBJDIR)/,$(OBJS))

#
#	rules
#

$(OBJDIR)/%.o : %.c
	$(CC) $(COPTS) $< -o $@

$(OBJDIR)/%.o : %.cpp
	$(CXX) $(COPTS) $< -o $@

all :	objs $(OBJDIR)/usbmouse

objs:
	mkdir objs

$(OBJDIR)/usbmouse: $(OBJS)
	$(CXX) -plugin -Xlinker -Bsymbolic -nodefaultlibs $(OBJS) -o $(OBJDIR)/usbmouse -lpyro

# ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

dist: all inst clean

inst:
	mkdir -p $(PYRO_BINARY_DESTINATION)/System/drivers/appserver/input
	cp -pf $(OBJDIR)/usbmouse $(PYRO_BINARY_DESTINATION)/System/drivers/appserver/input/
	addattrib $(PYRO_BINARY_DESTINATION)/System/drivers/appserver/input/usbmouse "CheckSum::MD5" `md5sum $(PYRO_BINARY_DESTINATION)/System/drivers/appserver/input/usbmouse | cut -c1-32`
	sync

clean:
	rm -rf $(OBJDIR)
	sync
