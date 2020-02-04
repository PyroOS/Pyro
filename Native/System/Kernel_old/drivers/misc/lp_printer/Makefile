CFLAGS   += -kernel -fno-PIC -c -O
AFLAGS   = -c

C_INCLUDE_PATH=$(PYRO_INCLUDE)

OBJS := lp.o
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

all :	objs $(OBJDIR)/lp      

objs:
	mkdir objs

$(OBJDIR)/lp: $(OBJS) Makefile
	$(CC) -kernel $(OBJS) -o $(OBJDIR)/lp      

# ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

dist: all inst clean

inst:
	mkdir -p $(PYRO_BINARY_DESTINATION)/System/drivers/dev/printer
	cp -f $(OBJDIR)/lp $(PYRO_BINARY_DESTINATION)/System/drivers/dev/printer/
	addattrib $(PYRO_BINARY_DESTINATION)/System/drivers/dev/printer/lp "CheckSum::MD5" `md5sum $(PYRO_BINARY_DESTINATION)/System/drivers/dev/printer/lp | cut -c1-32`
	sync

clean:
	rm -rf $(OBJDIR)
	sync
