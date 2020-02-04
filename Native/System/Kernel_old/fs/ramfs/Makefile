CFLAGS   += -c -kernel
AFLAGS   = -c

C_INCLUDE_PATH=$(PYRO_INCLUDE)

OBJS := ramfs.o


OBJDIR	:= objs
OBJS	:= $(addprefix $(OBJDIR)/,$(OBJS))
FS = ramfs

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

all : objs $(OBJDIR)/$(FS)

objs:
	mkdir objs

$(OBJDIR)/$(FS): $(OBJS) Makefile
	$(CC) $(OBJS) -kernel -o $(OBJDIR)/$(FS)

$(OBJDIR)/ramfs.o: ramfs.c
	$(CC) $(CFLAGS) -fno-PIC $< -o $@

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
