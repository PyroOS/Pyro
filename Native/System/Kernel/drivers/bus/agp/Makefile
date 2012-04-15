CFLAGS   += -kernel -fno-PIC -c -I. -O2 -Wall
AFLAGS   = -c

CFLAGS += -D__ENABLE_DEBUG__

C_INCLUDE_PATH=$(PYRO_INCLUDE)

OBJS := agp.o

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

all :	objs $(OBJDIR)/agp

objs:
	mkdir objs

$(OBJDIR)/agp: $(OBJS) Makefile
	$(CC) -kernel $(OBJS) -o $(OBJDIR)/agp

# ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

dist: all inst clean

inst:
	mkdir -p $(PYRO_BINARY_DESTINATION)/System/drivers/dev/bus
	cp -f $(OBJDIR)/agp $(PYRO_BINARY_DESTINATION)/System/drivers/dev/bus/
	addattrib $(PYRO_BINARY_DESTINATION)/System/drivers/dev/bus/agp "CheckSum::MD5" `md5sum $(PYRO_BINARY_DESTINATION)/System/drivers/dev/bus/agp | cut -c1-32`
	sync

clean:
	rm -rf $(OBJDIR)
	sync
