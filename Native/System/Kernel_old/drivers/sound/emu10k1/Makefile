DRIVER	:= emu10k1
OBJDIR	:= objs

C_INCLUDE_PATH=$(PYRO_INCLUDE)

all: $(OBJDIR) $(DRIVER)

$(OBJDIR):
	mkdir $(OBJDIR)

$(DRIVER):
	@cd src/ && $(MAKE)

clean:
	@cd src/ && $(MAKE) clean


# ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

dist: all inst clean

inst:
	mkdir -p $(PYRO_BINARY_DESTINATION)/System/drivers/dev/audio
	cp -f $(DRIVER) $(PYRO_BINARY_DESTINATION)/System/drivers/dev/audio/
	addattrib $(PYRO_BINARY_DESTINATION)/System/drivers/dev/audio/$(DRIVER) "CheckSum::MD5" `md5sum $(PYRO_BINARY_DESTINATION)/System/drivers/dev/audio/$(DRIVER) | cut -c1-32`
	sync
