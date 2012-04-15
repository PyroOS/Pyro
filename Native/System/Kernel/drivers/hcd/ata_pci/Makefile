CFLAGS   += -kernel -fno-PIC -c -I../../bus/ata
AFLAGS   = -c

C_INCLUDE_PATH=$(PYRO_INCLUDE)

# Enable the following to enable debug output
CFLAGS += -D__ENABLE_DEBUG__

OBJS := ata_pci.o ata_via.o ata_intel.o ata_sis.o ata_amd.o
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

all :	objs $(OBJDIR)/ata_pci

objs:
	mkdir objs

$(OBJDIR)/ata_pci: $(OBJS) Makefile
	$(CC) -kernel $(OBJS) -o $(OBJDIR)/ata_pci

# ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

dist: all inst clean

inst:
	mkdir -p $(PYRO_BINARY_DESTINATION)/System/drivers/dev/hcd
	cp -f $(OBJDIR)/ata_pci $(PYRO_BINARY_DESTINATION)/System/drivers/dev/hcd/
	addattrib $(PYRO_BINARY_DESTINATION)/System/drivers/dev/hcd/ata_pci "CheckSum::MD5" `md5sum $(PYRO_BINARY_DESTINATION)/System/drivers/dev/hcd/ata_pci | cut -c1-32`
	sync

clean:
	rm -rf $(OBJDIR)
	sync
