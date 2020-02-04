CFLAGS   += -kernel -D__LOADABLE__ -DUNSAFE_GET_VNODE -fno-PIC -c -fno-strict-aliasing -Wno-multichar -fno-exceptions
CXXFLAGS += -kernel -D__LOADABLE__ -DUNSAFE_GET_VNODE -fno-PIC -c -fno-strict-aliasing -Wno-multichar -fno-exceptions
AFLAGS   = -c

OBJS := BlockAllocator.o BPlusTree.o BufferPool.o Debug.o Index.o Inode.o \
		Journal.o Utility.o Volume.o kernel_interface.o

OBJDIR	:= objs
OBJS	:= $(addprefix $(OBJDIR)/,$(OBJS))
FS = bfs

#
#	rules
#

$(OBJDIR)/%.o : %.c
	$(CC) $(CFLAGS) $< -o $@

$(OBJDIR)/%.o : %.cpp
	$(CC) $(CXXFLAGS) $< -o $@

$(OBJDIR)/%.o : %.s
	$(CC) $(AFLAGS) -x assembler-with-cpp $< -o $@

$(OBJDIR)/%.o : %.S
	$(CC) $(CFLAGS) -x assembler-with-cpp $< -o $@

all : objs $(OBJDIR)/$(FS)

objs:
	mkdir objs

$(OBJDIR)/$(FS): $(OBJS) Makefile
	$(CC) -kernel $(OBJS) `$(CC) -kernel -print-libgcc-file-name` -o $(OBJDIR)/$(FS)

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
