CFLAGS   += -kernel -fno-PIC -c
AFLAGS   = -c

C_INCLUDE_PATH=$(PYRO_INCLUDE)

OBJS := driver.o
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

all :	objs $(OBJDIR)/radeon

objs:
	mkdir objs

$(OBJDIR)/radeon: $(OBJS) Makefile
	$(CC) -kernel $(OBJS) -o $(OBJDIR)/radeon

# ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

dist: all inst clean

inst:
	mkdir -p $(PYRO_BINARY_DESTINATION)/System/drivers/dev/graphics
	cp -f $(OBJDIR)/radeon $(PYRO_BINARY_DESTINATION)/System/drivers/dev/graphics/
	addattrib $(PYRO_BINARY_DESTINATION)/System/drivers/dev/graphics/radeon "CheckSum::MD5" `md5sum $(PYRO_BINARY_DESTINATION)/System/drivers/dev/graphics/radeon | cut -c1-32`
	sync

clean:
	rm -rf $(OBJDIR)
	sync
