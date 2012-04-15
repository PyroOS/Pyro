COPTS   = -I../../../server $(shell freetype-config --cflags) -c

OBJS =	warpspeeddecorator.o
OBJDIR	:= objs
OBJS	:= $(addprefix $(OBJDIR)/,$(OBJS))

#
#	rules
#

$(OBJDIR)/%.o : %.c
	$(CC) $(COPTS) $< -o $@

$(OBJDIR)/%.o : %.cpp
	$(CXX) $(COPTS) $< -o $@

all :	objs $(OBJDIR)/WarpSpeed

objs:
	mkdir objs

$(OBJDIR)/WarpSpeed: $(OBJS)
	$(CXX) -plugin -Xlinker -Bsymbolic $(OBJS) -nodefaultlibs -o $(OBJDIR)/WarpSpeed

# ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

dist: all inst clean

inst:
	mkdir -p $(PYRO_BINARY_DESTINATION)/System/drivers/appserver/decorators
	cp -pf $(OBJDIR)/WarpSpeed $(PYRO_BINARY_DESTINATION)/System/drivers/appserver/decorators/
	addattrib $(PYRO_BINARY_DESTINATION)/System/drivers/appserver/decorators/WarpSpeed "CheckSum::MD5" `md5sum $(PYRO_BINARY_DESTINATION)/System/drivers/appserver/decorators/WarpSpeed | cut -c1-32`
	sync

clean:
	rm -rf $(OBJDIR)
	sync
