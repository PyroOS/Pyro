CFLAGS   += -kernel -fno-PIC -c
AFLAGS   = -c

C_INCLUDE_PATH=$(PYRO_INCLUDE)

OBJS := random.o
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

all :	objs $(OBJDIR)/random

objs:
	mkdir objs

$(OBJDIR)/random: $(OBJS) Makefile
	$(CC) -kernel $(OBJS) -o $(OBJDIR)/random

# ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

dist: all inst clean

inst:
	mkdir -p $(PYRO_BINARY_DESTINATION)/System/drivers/dev
	cp -f $(OBJDIR)/random $(PYRO_BINARY_DESTINATION)/System/drivers/dev/
	addattrib $(PYRO_BINARY_DESTINATION)/System/drivers/dev/random "CheckSum::MD5" `md5sum $(PYRO_BINARY_DESTINATION)/System/drivers/dev/random | cut -c1-32`
	sync

clean:
	rm -rf $(OBJDIR)
	sync
