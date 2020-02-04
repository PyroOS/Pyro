CFLAGS   += -kernel -fno-PIC -c -O

C_INCLUDE_PATH=$(PYRO_INCLUDE)

SOURCE_DIR=.
OBJS := eepro100.o
DRIVER := eepro100
OBJDIR	:= objs
OBJS	:= $(addprefix $(OBJDIR)/,$(OBJS))

$(OBJDIR)/%.o : $(SOURCE_DIR)/%.c
	$(CC) $(CFLAGS) $< -o $@

$(OBJDIR)/%.o : $(SOURCE_DIR)/%.cpp
	$(CXX) $(CFLAGS) $< -o $@

$(OBJDIR)/%.o : $(SOURCE_DIR)/%.s
	$(CC) $(CFLAGS) -x assembler-with-cpp $< -o $@

$(OBJDIR)/%.o : $(SOURCE_DIR)/%.S
	$(CC) $(CFLAGS) -x assembler-with-cpp $< -o $@

all :	objs $(OBJDIR)/eepro100

objs:
	mkdir objs

$(OBJDIR)/eepro100: $(OBJS) Makefile
	$(CC) -kernel $(OBJS) -o $(OBJDIR)/eepro100

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
