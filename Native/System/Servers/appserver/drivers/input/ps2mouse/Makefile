COPTS   = -I../../../server -c

OBJS =	ps2mouse.o
OBJDIR	:= objs
OBJS	:= $(addprefix $(OBJDIR)/,$(OBJS))

#
#	rules
#

$(OBJDIR)/%.o : %.c
	$(CC) $(COPTS) $< -o $@

$(OBJDIR)/%.o : %.cpp
	$(CXX) $(COPTS) $< -o $@

all :	objs $(OBJDIR)/ps2mouse

objs:
	mkdir objs

$(OBJDIR)/ps2mouse: $(OBJS)
	$(CXX) -plugin -Xlinker -Bsymbolic -nodefaultlibs $(OBJS) -o $(OBJDIR)/ps2mouse -lpyro

# ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

dist: all inst clean

inst:
	mkdir -p $(PYRO_BINARY_DESTINATION)/System/drivers/appserver/input
	cp -pf $(OBJDIR)/ps2mouse $(PYRO_BINARY_DESTINATION)/System/drivers/appserver/input/
	addattrib $(PYRO_BINARY_DESTINATION)/System/drivers/appserver/input/ps2mouse "CheckSum::MD5" `md5sum $(PYRO_BINARY_DESTINATION)/System/drivers/appserver/input/ps2mouse | cut -c1-32`
	sync

clean:
	rm -rf $(OBJDIR)
	sync
