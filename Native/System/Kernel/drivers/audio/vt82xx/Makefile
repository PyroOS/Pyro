CFLAGS   += -kernel -fno-PIC -c -I../
AFLAGS   = -c

C_INCLUDE_PATH=$(PYRO_INCLUDE)

OBJS := audio.o ac97audio.o vt82xx.o

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

all :	objs $(OBJDIR)/vt82xx

objs:
	mkdir objs

$(OBJDIR)/vt82xx: $(OBJS) Makefile
	$(CC) -kernel $(OBJS) -o $(OBJDIR)/vt82xx

# ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

dist: all inst clean

inst:
	mkdir -p $(PYRO_BINARY_DESTINATION)/System/drivers/dev/audio
	cp -f $(OBJDIR)/vt82xx $(PYRO_BINARY_DESTINATION)/System/drivers/dev/audio/
	addattrib $(PYRO_BINARY_DESTINATION)/System/drivers/dev/audio/vt82xx "CheckSum::MD5" `md5sum $(PYRO_BINARY_DESTINATION)/System/drivers/dev/audio/vt82xx | cut -c1-32`
	sync

clean:
	rm -rf $(OBJDIR)
	sync
