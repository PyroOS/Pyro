COPTS   = -I../../../server  -c -O2 -Wall

DRIVERNAME := sylvnc_mouse

DEVBIN = /system/drivers/appserver/input
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
	$(CXX) -plugin $(OBJS) -o $(OBJDIR)/$(DRIVERNAME) -lsyllable

deps:

install: $(OBJDIR)/$(DRIVERNAME)
	cp -f $(OBJDIR)/$(DRIVERNAME) $(DEVBIN)/$(DRIVERNAME)

dist: $(OBJDIR)/sermouse
	cp -f $(OBJDIR)/$(DRIVERNAME) $(DIST_DIR)/atheos/sys/drivers/appserver/input/$(DRIVERNAME)

clean:
	-rm $(OBJS) $(OBJDIR)/$(DRIVERNAME)

