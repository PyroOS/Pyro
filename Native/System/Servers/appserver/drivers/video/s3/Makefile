COPTS   = -I$(TOP)/src $(shell freetype-config --cflags) -c

OBJS =	virgedrv.o
OBJDIR	:= objs
OBJS	:= $(addprefix $(OBJDIR)/,$(OBJS))

#
#	rules
#

$(OBJDIR)/%.o : %.c
	$(CC) $(COPTS) $< -o $@

$(OBJDIR)/%.o : %.cpp
	$(CXX) $(COPTS) $< -o $@

all :	objs $(OBJDIR)/s3_virge

objs:
	mkdir objs

$(OBJDIR)/s3_virge: $(OBJS)
	$(CXX) -plugin -Xlinker -Bsymbolic -nodefaultlibs $(OBJS) -o $(OBJDIR)/s3_virge

# ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

dist: all inst clean

inst:
	mkdir -p $(PYRO_BINARY_DESTINATION)/System/drivers/appserver/video
	cp -pf $(OBJDIR)/s3_virge $(PYRO_BINARY_DESTINATION)/System/drivers/appserver/video/
	addattrib $(PYRO_BINARY_DESTINATION)/System/drivers/appserver/video/s3_virge "CheckSum::MD5" `md5sum $(PYRO_BINARY_DESTINATION)/System/drivers/appserver/video/s3_virge | cut -c1-32`
	sync

clean:
	rm -rf $(OBJDIR)
	sync
