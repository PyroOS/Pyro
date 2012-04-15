COPTS   = -I../../../server $(shell freetype-config --cflags) -c

OBJS =	BlueDecorator.o
OBJDIR	:= objs
OBJS	:= $(addprefix $(OBJDIR)/,$(OBJS))

#
#	rules
#

$(OBJDIR)/%.o : %.c
	$(CC) $(COPTS) $< -o $@

$(OBJDIR)/%.o : %.cpp
	$(CXX) $(COPTS) $< -o $@

all :	objs $(OBJDIR)/Blue

objs:
	mkdir objs

$(OBJDIR)/Blue: $(OBJS)
	$(CXX) -plugin -Xlinker -Bsymbolic $(OBJS) -nodefaultlibs -o $(OBJDIR)/Blue

# ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

dist: all inst clean

inst:
	mkdir -p $(PYRO_BINARY_DESTINATION)/System/drivers/appserver/decorators
	cp -pf $(OBJDIR)/Blue $(PYRO_BINARY_DESTINATION)/System/drivers/appserver/decorators/
	addattrib $(PYRO_BINARY_DESTINATION)/System/drivers/appserver/decorators/Blue "CheckSum::MD5" `md5sum $(PYRO_BINARY_DESTINATION)/System/drivers/appserver/decorators/Blue | cut -c1-32`
	sync

clean:
	rm -rf $(OBJDIR)
	sync
