COPTS   = -I../../../server $(shell freetype-config --cflags) -c

OBJS =	PhotonDecorator.o
OBJDIR	:= objs
OBJS	:= $(addprefix $(OBJDIR)/,$(OBJS))

#
#	rules
#

$(OBJDIR)/%.o : %.c
	$(CC) $(COPTS) $< -o $@

$(OBJDIR)/%.o : %.cpp
	$(CXX) $(COPTS) $< -o $@

all :	objs $(OBJDIR)/Photon

objs:
	mkdir objs

$(OBJDIR)/Photon: $(OBJS)
	$(CXX) -plugin -Xlinker -Bsymbolic $(OBJS) -nodefaultlibs -o $(OBJDIR)/Photon

# ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

dist: all inst clean

inst:
	mkdir -p $(PYRO_BINARY_DESTINATION)/System/drivers/appserver/decorators
	cp -pf $(OBJDIR)/Photon $(PYRO_BINARY_DESTINATION)/System/drivers/appserver/decorators/
	addattrib $(PYRO_BINARY_DESTINATION)/System/drivers/appserver/decorators/Photon "CheckSum::MD5" `md5sum $(PYRO_BINARY_DESTINATION)/System/drivers/appserver/decorators/Photon | cut -c1-32`
	sync

clean:
	rm -rf $(OBJDIR)
	sync
