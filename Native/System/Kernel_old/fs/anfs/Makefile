CFLAGS   += -kernel -fno-PIC -c -Wno-multichar
AFLAGS   = -c

C_INCLUDE_PATH=$(PYRO_INCLUDE)

FS		:= anfs
OBJS	:= anfs.o gcc34_memcpy.o

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

all :	objs $(OBJDIR)/$(FS)

objs:
	mkdir $(OBJDIR)

$(OBJDIR)/$(FS): $(OBJS) Makefile
	$(CC) -kernel $(OBJS) -o $(OBJDIR)/$(FS)

# ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

dist: all inst clean

inst:
	mkdir -p $(PYRO_BINARY_DESTINATION)/System/drivers/fs
	cp -f $(OBJDIR)/$(FS) $(PYRO_BINARY_DESTINATION)/System/drivers/fs/$(FS)
	addattrib $(PYRO_BINARY_DESTINATION)/System/drivers/fs/$(FS) "CheckSum::MD5" `md5sum $(PYRO_BINARY_DESTINATION)/System/drivers/fs/$(FS) | cut -c1-32`
	sync

clean:
	rm -rf $(OBJDIR)
	sync
