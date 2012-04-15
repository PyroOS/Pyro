CFLAGS   += -kernel -fno-PIC -c -O

C_INCLUDE_PATH=$(PYRO_INCLUDE)

OBJS := via.o
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

all :	objs $(OBJDIR)/via

objs:
	mkdir objs

$(OBJDIR)/via: $(OBJS) Makefile
	$(CC) -kernel $(OBJS) -o $(OBJDIR)/via

# ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

dist: all inst clean

inst:
	mkdir -p $(PYRO_BINARY_DESTINATION)/System/drivers/dev/net/eth
	cp -f $(OBJDIR)/via $(PYRO_BINARY_DESTINATION)/System/drivers/dev/net/eth/
	addattrib $(PYRO_BINARY_DESTINATION)/System/drivers/dev/net/eth/via "CheckSum::MD5" `md5sum $(PYRO_BINARY_DESTINATION)/System/drivers/dev/net/eth/via | cut -c1-32`
	sync

clean:
	rm -rf $(OBJDIR)
	sync
