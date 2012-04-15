CFLAGS   += -kernel -fno-PIC -c -O

C_INCLUDE_PATH=$(PYRO_INCLUDE)

OBJS := pcnet32.o
DRIVER := pcnet32
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

all :	objs $(OBJDIR)/pcnet32

objs:
	mkdir objs

$(OBJDIR)/pcnet32: $(OBJS) Makefile
	$(CC) -kernel $(OBJS) -o $(OBJDIR)/pcnet32

# ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

dist: all inst clean

inst:
	mkdir -p $(PYRO_BINARY_DESTINATION)/System/drivers/dev/net/eth
	cp -f $(OBJDIR)/$(DRIVER) $(PYRO_BINARY_DESTINATION)/System/drivers/dev/net/eth/
	addattrib $(PYRO_BINARY_DESTINATION)/System/drivers/dev/net/eth/$(DRIVER) "CheckSum::MD5" `md5sum $(PYRO_BINARY_DESTINATION)/System/drivers/dev/net/eth/$(DRIVER) | cut -c1-32`
	sync

clean:
	rm -rf $(OBJDIR)
	sync
