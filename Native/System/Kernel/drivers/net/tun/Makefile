CFLAGS   += -kernel -fno-PIC -c -D__ENABLE_DEBUG__ -I.

C_INCLUDE_PATH=$(PYRO_INCLUDE)

SOURCE_DIR=.
OBJS := tun_if.o tun_dev.o
DRIVER := tun
OBJDIR	:= objs
OBJS	:= $(addprefix $(OBJDIR)/,$(OBJS))

$(OBJDIR)/%.o : $(SOURCE_DIR)/%.c
	$(CC) $(CFLAGS) $< -o $@

all:	objs $(OBJDIR)/$(DRIVER)

objs:
	mkdir objs

$(OBJDIR)/$(DRIVER): $(OBJS) Makefile
	$(CC) -kernel $(OBJS) -o $@

# ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

dist: all inst clean

inst:
	mkdir -p $(PYRO_BINARY_DESTINATION)/System/drivers/dev/net/eth
	cp -f $(OBJDIR)/$(DRIVER) $(PYRO_BINARY_DESTINATION)/System/drivers/dev/net/eth/
	addattrib $(PYRO_BINARY_DESTINATION)/System/drivers/dev/net/eth/$(DRIVER) "CheckSum::MD5" `md5sum $(PYRO_BINARY_DESTINATION)/System/drivers/dev/net/eth/$(DRIVER) | cut -c1-32`
	sync

clean:
	rm -rf $(OBJDIR)
	sync
