COPTS   = -I../../../server -c

OBJS =	keyboard.o
OBJDIR	:= objs
OBJS	:= $(addprefix $(OBJDIR)/,$(OBJS))

#
#	rules
#

$(OBJDIR)/%.o : %.c
	$(CC) $(COPTS) $< -o $@

$(OBJDIR)/%.o : %.cpp
	$(CXX) $(COPTS) $< -o $@

all :	objs $(OBJDIR)/usbkeyboard

objs:
	mkdir objs

$(OBJDIR)/usbkeyboard: $(OBJS)
	$(CXX) -plugin -Xlinker -Bsymbolic -nodefaultlibs $(OBJS) -o $(OBJDIR)/usbkeyboard -lpyro

# ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

dist: all inst clean

inst:
	mkdir -p $(PYRO_BINARY_DESTINATION)/System/drivers/appserver/input
	cp -pf $(OBJDIR)/usbkeyboard $(PYRO_BINARY_DESTINATION)/System/drivers/appserver/input/
	addattrib $(PYRO_BINARY_DESTINATION)/System/drivers/appserver/input/usbkeyboard "CheckSum::MD5" `md5sum $(PYRO_BINARY_DESTINATION)/System/drivers/appserver/input/usbkeyboard | cut -c1-32`
	sync

clean:
	rm -rf $(OBJDIR)
	sync
