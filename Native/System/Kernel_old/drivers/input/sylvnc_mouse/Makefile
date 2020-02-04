CFLAGS   = -kernel -fno-PIC -c -O3 -Wall
AFLAGS   = -c

C_INCLUDE_PATH=$(PYRO_INCLUDE)

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

# ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

dist: all inst clean

inst:
	mkdir -p $(PYRO_BINARY_DESTINATION)Native/Extra/System/SylVNC/System/drivers/dev
	cp -pf $(OBJDIR)/$(DRIVERNAME) $(PYRO_BINARY_DESTINATION)Native/Extra/System/SylVNC/System/drivers/dev/
	addattrib $(PYRO_BINARY_DESTINATION)Native/Extra/System/SylVNC/System/drivers/dev/$(DRIVERNAME) "CheckSum::MD5" `md5sum $(PYRO_BINARY_DESTINATION)Native/Extra/System/SylVNC/System/drivers/dev/$(DRIVERNAME) | cut -c1-32`
	sync

clean:
	rm -rf $(OBJDIR)
	sync
