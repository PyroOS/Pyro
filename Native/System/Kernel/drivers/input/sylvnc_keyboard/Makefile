CFLAGS   = -kernel -fno-PIC -c -O3 -Wall
AFLAGS   = -c

C_INCLUDE_PATH=$(PYRO_INCLUDE)

OBJS := kbd.o

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

all :	objs $(OBJDIR)/keybd

objs:
	mkdir objs

$(OBJDIR)/keybd: $(OBJS) Makefile
	$(CC) -kernel $(OBJS) -o $(OBJDIR)/keybd

# ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

dist: all inst clean

inst:
	mkdir -p $(PYRO_BINARY_DESTINATION)Native/Extra/System/SylVNC/System/drivers/dev
	cp -pf $(OBJDIR)/keybd $(PYRO_BINARY_DESTINATION)Native/Extra/System/SylVNC/System/drivers/dev/sylvnc_keyboard
	addattrib $(PYRO_BINARY_DESTINATION)Native/Extra/System/SylVNC/System/drivers/dev/sylvnc_keyboard "CheckSum::MD5" `md5sum $(PYRO_BINARY_DESTINATION)Native/Extra/System/SylVNC/System/drivers/dev/sylvnc_keyboard | cut -c1-32`
	sync

clean:
	rm -rf $(OBJDIR)
	sync
