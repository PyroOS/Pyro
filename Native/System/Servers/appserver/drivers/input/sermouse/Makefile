COPTS   = -I../../../server -c

OBJS =	sermouse.o
OBJDIR	:= objs
OBJS	:= $(addprefix $(OBJDIR)/,$(OBJS))

#
#	rules
#

$(OBJDIR)/%.o : %.c
	$(CC) $(COPTS) $< -o $@

$(OBJDIR)/%.o : %.cpp
	$(CXX) $(COPTS) $< -o $@

all :	objs $(OBJDIR)/sermouse

objs:
	mkdir objs

$(OBJDIR)/sermouse: $(OBJS)
	$(CXX) -plugin -Xlinker -Bsymbolic -nodefaultlibs $(OBJS) -o $(OBJDIR)/sermouse -lpyro

# ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

dist: all inst clean

inst:
	mkdir -p $(PYRO_BINARY_DESTINATION)/System/drivers/appserver/input
	cp -pf $(OBJDIR)/sermouse $(PYRO_BINARY_DESTINATION)/System/drivers/appserver/input/
	addattrib $(PYRO_BINARY_DESTINATION)/System/drivers/appserver/input/sermouse "CheckSum::MD5" `md5sum $(PYRO_BINARY_DESTINATION)/System/drivers/appserver/input/sermouse | cut -c1-32`
	sync

clean:
	rm -rf $(OBJDIR)
	sync
