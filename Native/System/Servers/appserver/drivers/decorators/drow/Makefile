COPTS   = -I../../../server $(shell freetype-config --cflags) -c

OBJS =	drowdecorator.o
OBJDIR	:= objs
OBJS	:= $(addprefix $(OBJDIR)/,$(OBJS))

#
#	rules
#

$(OBJDIR)/%.o : %.c
	$(CC) $(COPTS) $< -o $@

$(OBJDIR)/%.o : %.cpp
	$(CXX) $(COPTS) $< -o $@

all :	objs $(OBJDIR)/Drow

objs:
	mkdir objs

$(OBJDIR)/Drow: $(OBJS)
	$(CXX) -plugin -Xlinker -Bsymbolic $(OBJS) -nodefaultlibs -o $(OBJDIR)/Drow

# ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

dist: all inst clean

inst:
	mkdir -p $(PYRO_BINARY_DESTINATION)/System/drivers/appserver/decorators
	cp -pf $(OBJDIR)/Drow $(PYRO_BINARY_DESTINATION)/System/drivers/appserver/decorators/
	addattrib $(PYRO_BINARY_DESTINATION)/System/drivers/appserver/decorators/Drow "CheckSum::MD5" `md5sum $(PYRO_BINARY_DESTINATION)/System/drivers/appserver/decorators/Drow | cut -c1-32`
	sync

clean:
	rm -rf $(OBJDIR)
	sync
