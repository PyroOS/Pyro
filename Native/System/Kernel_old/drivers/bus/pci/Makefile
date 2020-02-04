CFLAGS   += -Wstrict-prototypes -kernel -fno-PIC -c -fno-strict-aliasing -Wno-trigraphs -fno-common -D__ENABLE_DEBUG__ -I../acpi/include
AFLAGS   = -c

C_INCLUDE_PATH=$(PYRO_INCLUDE)

OBJS := pci.o pci_acpi_link.o pci_acpi_router.o pci_registers.o

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

all :	objs $(OBJDIR)/pci

objs:
	mkdir objs

$(OBJDIR)/pci: $(OBJS) Makefile
	$(CC) -kernel $(OBJS) -o $(OBJDIR)/pci

# ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

dist: all inst clean

inst:
	mkdir -p $(PYRO_BINARY_DESTINATION)/System/drivers/dev/bus
	cp -f $(OBJDIR)/pci $(PYRO_BINARY_DESTINATION)/System/drivers/dev/bus/
	addattrib $(PYRO_BINARY_DESTINATION)/System/drivers/dev/bus/pci "CheckSum::MD5" `md5sum $(PYRO_BINARY_DESTINATION)/System/drivers/dev/bus/pci | cut -c1-32`
	sync

clean:
	rm -rf $(OBJDIR)
	sync
