# Makefile for Meter.so

COPTS = -c -fexceptions -ffast-math

OBJS = meter.o
OBJDIR := objs
OBJS	:= $(addprefix $(OBJDIR)/,$(OBJS))

# Rules
$(OBJDIR)/%.o : %.c
	@echo Compiling : $<
	@$(CC) $(COPTS) $< -o $@

$(OBJDIR)/%.o : %.cpp
	@echo Compiling : $<
	@$(CXX) $(COPTS) $< -o $@

$(OBJDIR)/%.o : %.s
	@echo Assembling : $<
	@$(CC) $(COPTS) -x assembler-with-cpp $< -o $@

all : objs Meter.so

objs:
	@mkdir -p objs

Meter.so: $(OBJS)
	@echo Linking...
	@$(CXX) -shared -Xlinker -soname=Meter.so $(OBJS) -o Meter.so -lpyro
	@echo Adding resources...
	@rescopy Meter.so -r images/*.png 

# ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

dist: all inst clean

inst:
	mkdir -p $(PYRO_BINARY_DESTINATION)/System/extensions/dock
	cp -f Meter.so $(PYRO_BINARY_DESTINATION)/System/extensions/dock/
	addattrib $(PYRO_BINARY_DESTINATION)/System/extensions/dock/Meter.so "CheckSum::MD5" `md5sum $(PYRO_BINARY_DESTINATION)/System/extensions/dock/Meter.so | cut -c1-32`
	sync

clean:
	rm -rf $(OBJDIR)
	rm -f Meter.so
	sync
