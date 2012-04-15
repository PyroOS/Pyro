CFLAGS   += -kernel -fno-PIC -c

C_INCLUDE_PATH=$(PYRO_INCLUDE)

OBJS := ne2000.o
OBJDIR	:= objs
OBJS	:= $(addprefix $(OBJDIR)/,$(OBJS))

$(OBJDIR)/%.o : %.c
	$(CC) $(CFLAGS) $< -o $@

$(OBJDIR)/%.o : %.cpp
	$(CXX) $(CFLAGS) $< -o $@

$(OBJDIR)/%.o : %.s
	$(CC) $(CFLAGS) -x assembler-with-cpp $< -o $@

$(OBJDIR)/%.o : %.S
	$(CC) $(CFLAGS) -x assembler-with-cpp $< -o $@

all :	objs $(OBJDIR)/ne2000

objs:
	mkdir objs

$(OBJDIR)/ne2000: $(OBJS) Makefile
	$(CC) -kernel $(OBJS) -o $(OBJDIR)/ne2000

# ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

dist: all inst clean

inst:
	mkdir -p $(PYRO_BINARY_DESTINATION)/System/drivers/dev/net/eth
	cp -f $(OBJDIR)/ne2000 $(PYRO_BINARY_DESTINATION)/System/drivers/dev/net/eth/
	addattrib $(PYRO_BINARY_DESTINATION)/System/drivers/dev/net/eth/ne2000 "CheckSum::MD5" `md5sum $(PYRO_BINARY_DESTINATION)/System/drivers/dev/net/eth/ne2000 | cut -c1-32`
	sync

clean:
	rm -rf $(OBJDIR)
	sync
