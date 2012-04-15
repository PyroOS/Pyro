COPTS   = -I$(TOP)/src $(shell freetype-config --cflags) -O2 -c -Wall

DRIVERNAME=sylvnc

OBJS =	$(DRIVERNAME).o

OBJDIR	:= objs
OBJS	:= $(addprefix $(OBJDIR)/,$(OBJS))

DRIVER := $(OBJDIR)/$(DRIVERNAME)

all :	objs $(OBJDIR)/$(DRIVERNAME)

objs:
	mkdir objs

$(OBJDIR)/$(DRIVERNAME): $(OBJS)
	$(CXX) -plugin $(OBJS) -o $(OBJDIR)/$(DRIVERNAME) -lvncserver -ljpeg -lsyllable 

$(OBJDIR)/$(DRIVERNAME).o: $(DRIVERNAME).cpp $(DRIVERNAME).h
	$(CC) $(COPTS) $(DRIVERNAME).cpp -o $@


deps:

install: $(DRIVER)
	cp -f $(DRIVER) /system/drivers/appserver/video/

dist: $(DRIVER)
	cp -f $(DRIVER) $(DIST_DIR)/atheos/sys/drivers/appserver/video/

clean:
	-rm $(OBJS) $(OBJDIR)/$(DRIVERNAME)

#install:
#	cp -f $(OBJDIR)/$(DRIVERNAME) /system/drivers/appserver/video/

