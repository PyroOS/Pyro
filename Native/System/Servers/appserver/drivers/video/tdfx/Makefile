COPTS   = -I$(TOP)/src $(shell freetype-config --cflags) -c

OBJS =	tdfx.o gtf.o
OBJDIR	:= objs
OBJS	:= $(addprefix $(OBJDIR)/,$(OBJS))

#
#	rules
#

$(OBJDIR)/%.o : %.c
	$(CC) $(COPTS) $< -o $@

$(OBJDIR)/%.o : %.cpp
	$(CXX) $(COPTS) $< -o $@

all :	objs $(OBJDIR)/tdfx

objs:
	mkdir objs

$(OBJDIR)/tdfx: $(OBJS)
	$(CXX) -plugin $(OBJS) -o $(OBJDIR)/tdfx -lpyro

# ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

dist: all inst clean

inst:
	mkdir -p $(PYRO_BINARY_DESTINATION)/System/drivers/appserver/video
	cp -pf $(OBJDIR)/tdfx $(PYRO_BINARY_DESTINATION)/System/drivers/appserver/video/
	addattrib $(PYRO_BINARY_DESTINATION)/System/drivers/appserver/video/tdfx "CheckSum::MD5" `md5sum $(PYRO_BINARY_DESTINATION)/System/drivers/appserver/video/tdfx | cut -c1-32`
	sync

clean:
	rm -rf $(OBJDIR)
	sync
