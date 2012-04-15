COPTS   = -I../../../server $(shell freetype-config --cflags) -c

OBJS =	Win98Decorator.o
OBJDIR	:= objs
OBJS	:= $(addprefix $(OBJDIR)/,$(OBJS))

#
#	rules
#

$(OBJDIR)/%.o : %.c
	$(CC) $(COPTS) $< -o $@

$(OBJDIR)/%.o : %.cpp
	$(CXX) $(COPTS) $< -o $@

all :	objs $(OBJDIR)/Win98

objs:
	mkdir objs

$(OBJDIR)/Win98: $(OBJS)
	$(CXX) -plugin -Xlinker -Bsymbolic $(OBJS) -nodefaultlibs -o $(OBJDIR)/Win98

# ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

dist: all inst clean

inst:
	mkdir -p $(PYRO_BINARY_DESTINATION)/System/drivers/appserver/decorators
	cp -pf $(OBJDIR)/Win98 $(PYRO_BINARY_DESTINATION)/System/drivers/appserver/decorators/
	addattrib $(PYRO_BINARY_DESTINATION)/System/drivers/appserver/decorators/Win98 "CheckSum::MD5" `md5sum $(PYRO_BINARY_DESTINATION)/System/drivers/appserver/decorators/Win98 | cut -c1-32`
	sync

clean:
	rm -rf $(OBJDIR)
	sync
