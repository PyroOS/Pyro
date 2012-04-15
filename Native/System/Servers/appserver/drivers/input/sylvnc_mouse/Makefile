COPTS   = -I../../../server  -c -O2 -Wall

DRIVERNAME := sylvnc_mouse

OBJS =	$(DRIVERNAME).o

OBJDIR	:= objs
OBJS	:= $(addprefix $(OBJDIR)/,$(OBJS))

#
#	rules
#

$(OBJDIR)/%.o : %.c
	$(CC) $(COPTS) $< -o $@

$(OBJDIR)/%.o : %.cpp
	$(CC) $(COPTS) $< -o $@

all :	objs $(OBJDIR)/$(DRIVERNAME)

objs:
	mkdir objs

$(OBJDIR)/$(DRIVERNAME): $(OBJS)
	$(CXX) -plugin $(OBJS) -o $(OBJDIR)/$(DRIVERNAME) -lpyro

# ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

dist: all inst clean

inst:
	mkdir -p $(PYRO_BINARY_DESTINATION)Native/Extra/System/SylVNC/System/drivers/appserver/input
	cp -pf $(OBJDIR)/$(DRIVERNAME) $(PYRO_BINARY_DESTINATION)Native/Extra/System/SylVNC/System/drivers/appserver/input/
	addattrib $(PYRO_BINARY_DESTINATION)Native/Extra/System/SylVNC/System/drivers/appserver/input/$(DRIVERNAME) "CheckSum::MD5" `md5sum $(PYRO_BINARY_DESTINATION)Native/Extra/System/SylVNC/System/drivers/appserver/input/$(DRIVERNAME) | cut -c1-32`
	sync

clean:
	rm -rf $(OBJDIR)
	sync
