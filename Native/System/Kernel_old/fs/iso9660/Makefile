CFLAGS   += -kernel -fno-strict-aliasing
AFLAGS   = -c

#CFLAGS += -I.. -I../../../include

OBJS := iso.o kernel_interface.o

FS = iso9660

all: $(OBJS)
	$(CC) -nostdlib -shared -L../../kernel/objs $(OBJS) -o $(FS) -lkernel `gcc -print-libgcc-file-name`

# ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

dist: all inst clean

inst:
	mkdir -p $(PYRO_BINARY_DESTINATION)/System/drivers/fs
	cp -f $(FS) $(PYRO_BINARY_DESTINATION)/System/drivers/fs/$(FS)
	addattrib $(PYRO_BINARY_DESTINATION)/System/drivers/fs/$(FS) "CheckSum::MD5" `md5sum $(PYRO_BINARY_DESTINATION)/System/drivers/fs/$(FS) | cut -c1-32`
	sync

clean:
	rm -f $(OBJS)
	rm -f $(FS)
	sync
