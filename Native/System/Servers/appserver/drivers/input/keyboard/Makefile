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

all :	objs $(OBJDIR)/keyboard

objs:
	mkdir objs

$(OBJDIR)/keyboard: $(OBJS)
	$(CXX) -plugin -Xlinker -Bsymbolic -nodefaultlibs $(OBJS) -o $(OBJDIR)/keyboard -lpyro

# ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

dist: all inst clean

inst:
	mkdir -p $(PYRO_BINARY_DESTINATION)/System/drivers/appserver/input
	cp -pf $(OBJDIR)/keyboard $(PYRO_BINARY_DESTINATION)/System/drivers/appserver/input/
	addattrib $(PYRO_BINARY_DESTINATION)/System/drivers/appserver/input/keyboard "CheckSum::MD5" `md5sum $(PYRO_BINARY_DESTINATION)/System/drivers/appserver/input/keyboard | cut -c1-32`
	sync

clean:
	rm -rf $(OBJDIR)
	sync
