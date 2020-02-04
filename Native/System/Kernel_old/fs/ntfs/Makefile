CFLAGS   += -O -I. -c -fno-strict-aliasing -kernel -D__ENABLE_DEBUG__

C_INCLUDE_PATH=$(PYRO_INCLUDE)

FS		:= ntfs
OBJS	:= ntfs.o util.o inode.o attr.o unistr.o support.o super.o dir.o

OBJDIR	:= objs
OBJS	:= $(addprefix $(OBJDIR)/,$(OBJS))

#
#	rules
#

$(OBJDIR)/%.o : %.c
	$(CC) $(CFLAGS) $< -o $@

all :	objs $(OBJDIR)/$(FS)

objs:
	mkdir objs

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
