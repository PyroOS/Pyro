CFLAGS   += -kernel -fno-PIC -c -I.
AFLAGS   = -c

CFLAGS += -D__ENABLE_DEBUG__

C_INCLUDE_PATH=$(PYRO_INCLUDE)

OBJS := scsi.o scsi_common.o scsi_generic.o scsi_disk.o scsi_cdrom.o

OBJDIR	:= objs
OBJS	:= $(addprefix $(OBJDIR)/,$(OBJS))

#
#	rules
#

$(OBJDIR)/%.o : %.c
	$(CC) $(CFLAGS) $< -o $@

$(OBJDIR)/%.o : %.cpp
	$(CXX) $(CFLAGS) $< -o $@

$(OBJDIR)/%.o : %.s
	$(CC) $(AFLAGS) -x assembler-with-cpp $< -o $@

$(OBJDIR)/%.o : %.S
	$(CC) $(CFLAGS) -x assembler-with-cpp $< -o $@

all :	objs $(OBJDIR)/scsi

objs:
	mkdir objs

$(OBJDIR)/scsi: $(OBJS) Makefile
	$(CC) -kernel $(OBJS) -o $(OBJDIR)/scsi

# ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

dist: all inst clean

inst:
	mkdir -p $(PYRO_BINARY_DESTINATION)/System/drivers/dev/bus
	cp -f $(OBJDIR)/scsi $(PYRO_BINARY_DESTINATION)/System/drivers/dev/bus/
	addattrib $(PYRO_BINARY_DESTINATION)/System/drivers/dev/bus/scsi "CheckSum::MD5" `md5sum $(PYRO_BINARY_DESTINATION)/System/drivers/dev/bus/scsi | cut -c1-32`
	sync

clean:
	rm -rf $(OBJDIR)
	sync
