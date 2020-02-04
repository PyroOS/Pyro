CFLAGS   += -kernel -fno-PIC -c
AFLAGS   = -c

C_INCLUDE_PATH=$(PYRO_INCLUDE)

# Enable the following to enable debug output
CFLAGS += -D__ENABLE_DEBUG__

OBJS := ata.o ata_port.o ata_probe.o ata_io.o ata_command.o ata_drive.o atapi_drive.o atapi.o

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

all :	objs $(OBJDIR)/ata 

objs:
	mkdir objs

$(OBJDIR)/ata: $(OBJS) Makefile
	$(CC) -kernel $(OBJS) -o $(OBJDIR)/ata

# ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

dist: all inst clean

inst:
	mkdir -p $(PYRO_BINARY_DESTINATION)/System/drivers/dev/bus
	cp -f $(OBJDIR)/ata $(PYRO_BINARY_DESTINATION)/System/drivers/dev/bus/
	addattrib $(PYRO_BINARY_DESTINATION)/System/drivers/dev/bus/ata "CheckSum::MD5" `md5sum $(PYRO_BINARY_DESTINATION)/System/drivers/dev/bus/ata | cut -c1-32`
	sync

clean:
	rm -rf $(OBJDIR)
	sync
