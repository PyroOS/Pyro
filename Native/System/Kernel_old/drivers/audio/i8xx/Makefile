CFLAGS   += -kernel -fno-PIC -c -I../
AFLAGS   = -c

C_INCLUDE_PATH=$(PYRO_INCLUDE)

OBJS := audio.o ac97audio.o i8xx.o

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

all :	objs $(OBJDIR)/i8xx

objs:
	mkdir objs

$(OBJDIR)/i8xx: $(OBJS) Makefile
	$(CC) -kernel $(OBJS) -o $(OBJDIR)/i8xx

# ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

dist: all inst clean

inst:
	mkdir -p $(PYRO_BINARY_DESTINATION)/System/drivers/dev/audio
	cp -f $(OBJDIR)/i8xx $(PYRO_BINARY_DESTINATION)/System/drivers/dev/audio/
	addattrib $(PYRO_BINARY_DESTINATION)/System/drivers/dev/audio/i8xx "CheckSum::MD5" `md5sum $(PYRO_BINARY_DESTINATION)/System/drivers/dev/audio/i8xx | cut -c1-32`
	sync

clean:
	rm -rf $(OBJDIR)
	sync
