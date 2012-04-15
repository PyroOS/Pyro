COPTS   = -I$(TOP)/src $(shell freetype-config --cflags) -c

OBJS =	riva.o riva_hw.o
OBJDIR	:= objs
OBJS	:= $(addprefix $(OBJDIR)/,$(OBJS))

#
#	rules
#

$(OBJDIR)/%.o : %.c
	$(CC) $(COPTS) $< -o $@

$(OBJDIR)/%.o : %.cpp
	$(CXX) $(COPTS) $< -o $@

all :	objs $(OBJDIR)/riva

objs:
	mkdir objs

$(OBJDIR)/riva: $(OBJS)
	$(CXX) -plugin -Xlinker -Bsymbolic -nodefaultlibs $(OBJS) -o $(OBJDIR)/riva

# ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

dist: all inst clean

inst:
	mkdir -p $(PYRO_BINARY_DESTINATION)/System/drivers/appserver/video
	cp -pf $(OBJDIR)/riva $(PYRO_BINARY_DESTINATION)/System/drivers/appserver/video/
	addattrib $(PYRO_BINARY_DESTINATION)/System/drivers/appserver/video/riva "CheckSum::MD5" `md5sum $(PYRO_BINARY_DESTINATION)/System/drivers/appserver/video/riva | cut -c1-32`
	sync

clean:
	rm -rf $(OBJDIR)
	sync
