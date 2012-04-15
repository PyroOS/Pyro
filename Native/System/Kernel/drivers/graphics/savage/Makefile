CFLAGS   = -kernel -fno-PIC -c -I.
AFLAGS   = -c

OBJS := savage.o
OBJDIR	:= objs
OBJS	:= $(addprefix $(OBJDIR)/,$(OBJS))

#
#	rules
#

$(OBJDIR)/%.o : %.c
	$(CC) $(CFLAGS) $< -o $@

$(OBJDIR)/%.o : %.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

$(OBJDIR)/%.o : %.s
	$(CC) $(AFLAGS) -x assembler-with-cpp $< -o $@

$(OBJDIR)/%.o : %.S
	$(CC) $(CFLAGS) -x assembler-with-cpp $< -o $@

all :	objs $(OBJDIR)/savage

objs:
	mkdir objs

$(OBJDIR)/savage: $(OBJS) Makefile
	$(CC) -kernel $(OBJS) -o $(OBJDIR)/savage

# ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

dist: all inst clean

inst:
	mkdir -p $(PYRO_BINARY_DESTINATION)/System/drivers/dev/graphics
	cp -f $(OBJDIR)/savage $(PYRO_BINARY_DESTINATION)/System/drivers/dev/graphics/
	addattrib $(PYRO_BINARY_DESTINATION)/System/drivers/dev/graphics/savage "CheckSum::MD5" `md5sum $(PYRO_BINARY_DESTINATION)/System/drivers/dev/graphics/savage | cut -c1-32`
	sync

clean:
	rm -rf $(OBJDIR)
	sync
