COPTS   = -I$(TOP)/src $(shell freetype-config --cflags) -c

OBJS =	s3_chrome.o
OBJDIR	:= objs
OBJS	:= $(addprefix $(OBJDIR)/,$(OBJS))

#
#	rules
#

$(OBJDIR)/%.o : %.c
	$(CC) $(COPTS) $< -o $@

$(OBJDIR)/%.o : %.cpp
	$(CXX) $(COPTS) $< -o $@

all :	objs $(OBJDIR)/s3_chrome

objs:
	mkdir objs

$(OBJDIR)/s3_chrome: $(OBJS)
	$(CXX) -plugin -Xlinker -Bsymbolic -nodefaultlibs $(OBJS) -o $(OBJDIR)/s3_chrome

# ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

dist: all inst clean

inst:
	mkdir -p $(PYRO_BINARY_DESTINATION)/System/drivers/appserver/video
	cp -pf $(OBJDIR)/s3_chrome $(PYRO_BINARY_DESTINATION)/System/drivers/appserver/video/
	addattrib $(PYRO_BINARY_DESTINATION)/System/drivers/appserver/video/s3_chrome "CheckSum::MD5" `md5sum $(PYRO_BINARY_DESTINATION)/System/drivers/appserver/video/s3_chrome | cut -c1-32`
	sync

clean:
	rm -rf $(OBJDIR)
	sync
