COPTS   = -I$(TOP)/src $(shell freetype-config --cflags) -c

DRIVERNAME=sylvnc

OBJS =	$(DRIVERNAME).o

OBJDIR	:= objs
OBJS	:= $(addprefix $(OBJDIR)/,$(OBJS))

DRIVER := $(OBJDIR)/$(DRIVERNAME)

all :	objs $(OBJDIR)/$(DRIVERNAME)

objs:
	mkdir objs

$(OBJDIR)/$(DRIVERNAME): $(OBJS)
	$(CXX) -plugin $(OBJS) -o $(OBJDIR)/$(DRIVERNAME) -lvncserver -ljpeg -lpyro

$(OBJDIR)/$(DRIVERNAME).o: $(DRIVERNAME).cpp $(DRIVERNAME).h
	$(CC) $(COPTS) $(DRIVERNAME).cpp -o $@

# ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

dist: $(DRIVER) inst clean

inst:
	mkdir -p $(PYRO_BINARY_DESTINATION)Native/Extra/System/SylVNC/System/drivers/appserver/video
	cp -pf $(OBJDIR)/$(DRIVERNAME) $(PYRO_BINARY_DESTINATION)Native/Extra/System/SylVNC/System/drivers/appserver/video/sylvnc_video
	addattrib $(PYRO_BINARY_DESTINATION)Native/Extra/System/SylVNC/System/drivers/appserver/video/sylvnc_video "CheckSum::MD5" `md5sum $(PYRO_BINARY_DESTINATION)Native/Extra/System/SylVNC/System/drivers/appserver/video/sylvnc_video | cut -c1-32`
	sync

clean:
	rm -rf $(OBJDIR)
	sync
