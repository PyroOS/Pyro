COPTS   = -I$(TOP)/src $(shell freetype-config --cflags) -c

OBJS = init301.o init.o sis_video.o sis.o sis3xx.o
OBJDIR	:= objs
OBJS	:= $(addprefix $(OBJDIR)/,$(OBJS))

#
#	rules
#

$(OBJDIR)/%.o : %.c
	$(CC) $(COPTS) $< -o $@

$(OBJDIR)/%.o : %.cpp
	$(CXX) $(COPTS) $< -o $@

all :	objs $(OBJDIR)/sis3xx

objs:
	mkdir objs

$(OBJDIR)/sis3xx: $(OBJS)
	$(CXX) -plugin -Xlinker -Bsymbolic -nodefaultlibs $(OBJS) -o $(OBJDIR)/sis3xx

# ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

dist: all inst clean

inst:
	mkdir -p $(PYRO_BINARY_DESTINATION)/System/drivers/appserver/video
	cp -pf $(OBJDIR)/sis3xx $(PYRO_BINARY_DESTINATION)/System/drivers/appserver/video/
	addattrib $(PYRO_BINARY_DESTINATION)/System/drivers/appserver/video/sis3xx "CheckSum::MD5" `md5sum $(PYRO_BINARY_DESTINATION)/System/drivers/appserver/video/sis3xx | cut -c1-32`
	sync

clean:
	rm -rf $(OBJDIR)
	sync
