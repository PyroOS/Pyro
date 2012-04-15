COPTS   = -I$(TOP)/src $(shell freetype-config --cflags) -c

CFGDIR = $(PYRO_BINARY_DESTINATION)/system/config/drivers/appserver/video
OBJS = radeon.o radeon_accel.o radeon_misc.o radeon_monitor.o radeon_pll.o radeon_rom.o radeon_video.o
OBJDIR	:= objs
OBJS	:= $(addprefix $(OBJDIR)/,$(OBJS))

#
#	rules
#

$(OBJDIR)/%.o : %.c
	$(CC) $(COPTS) $< -o $@

$(OBJDIR)/%.o : %.cpp
	$(CXX) $(COPTS) $< -o $@

all :	objs $(OBJDIR)/radeon

objs:
	mkdir objs

$(OBJDIR)/radeon: $(OBJS)
	$(CXX) -plugin -Xlinker -Bsymbolic -nodefaultlibs $(OBJS) -o $(OBJDIR)/radeon

# ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

dist: all inst clean

inst:
	mkdir -p $(PYRO_BINARY_DESTINATION)/System/drivers/appserver/video
	cp -pf $(OBJDIR)/radeon $(PYRO_BINARY_DESTINATION)/System/drivers/appserver/video/
	addattrib $(PYRO_BINARY_DESTINATION)/System/drivers/appserver/video/radeon "CheckSum::MD5" `md5sum $(PYRO_BINARY_DESTINATION)/System/drivers/appserver/video/radeon | cut -c1-32`
	sync

clean:
	rm -rf $(OBJDIR)
	sync
