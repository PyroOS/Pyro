CFLAGS   = -kernel -fno-PIC -c -O3 -Wall
AFLAGS   = -c

C_INCLUDE_PATH=$(ATHEOS_INCLUDE_PATH)

DRIVERNAME := sylvnc_mouse

OBJS := $(DRIVERNAME).o


OBJDIR	:= objs
OBJS	:= $(addprefix $(OBJDIR)/,$(OBJS))


#
#	rules
#

$(OBJDIR)/%.o : %.c
	$(CC) $(CFLAGS) $< -o $@

$(OBJDIR)/%.o : %.cpp
	$(CC) $(CFLAGS) $< -o $@

$(OBJDIR)/%.o : %.s
	$(CC) $(AFLAGS) -x assembler-with-cpp $< -o $@

$(OBJDIR)/%.o : %.S
	$(CC) $(CFLAGS) -x assembler-with-cpp $< -o $@




all :	objs $(OBJDIR)/$(DRIVERNAME)

objs:
	mkdir objs

$(OBJDIR)/$(DRIVERNAME): $(OBJS) Makefile
	$(CC) -kernel $(OBJS) -o $(OBJDIR)/$(DRIVERNAME)

install: all
	cp -f $(OBJDIR)/$(DRIVERNAME) /system/drivers/dev/input/$(DRIVERNAME)

dist: all
	cp -f $(OBJDIR)/$(DRIVERNAME) $(DIST_DIR)/atheos/sys/drivers/dev/$(DRIVERNAME)

clean:
	-rm -f $(OBJS) $(OBJDIR)/$(DRIVERNAME)

