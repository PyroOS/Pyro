CFLAGS   += -kernel -fno-PIC -c -I. -D__ENABLE_DEBUG__
#CFLAGS   += -DDEBUG_LIMIT=KERN_INFO
AFLAGS   = -c

C_INCLUDE_PATH=$(PYRO_INCLUDE)

OBJS := maestro.o
DRIVER := maestro
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

#
# targets
#

all :	objs $(OBJDIR)/$(DRIVER)

objs:
	mkdir $(OBJDIR)

$(OBJDIR)/$(DRIVER): $(OBJS) Makefile
	$(CC) -kernel $(OBJS) -o $@

# ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

dist: all inst clean

inst:
	mkdir -p $(PYRO_BINARY_DESTINATION)/System/drivers/dev/audio
	cp -f $(OBJDIR)/$(DRIVER) $(PYRO_BINARY_DESTINATION)/System/drivers/dev/audio/
	addattrib $(PYRO_BINARY_DESTINATION)/System/drivers/dev/audio/$(DRIVER) "CheckSum::MD5" `md5sum $(PYRO_BINARY_DESTINATION)/System/drivers/dev/audio/$(DRIVER) | cut -c1-32`
	sync

clean:
	rm -rf $(OBJDIR)
	sync
