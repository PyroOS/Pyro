CFLAGS   += -kernel -fno-PIC -c
AFLAGS   = -c

C_INCLUDE_PATH=$(PYRO_INCLUDE)

OBJS := ac97_codec.o via686a_io.o via686a.o
DRIVER := via686a
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

all :	objs $(OBJDIR)/via686a

objs:
	mkdir objs

$(OBJDIR)/via686a: $(OBJS) Makefile
	$(CC) -kernel $(OBJS) -o $(OBJDIR)/via686a

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
