# Makefile for Mixer.so

COPTS = -c -fexceptions -ffast-math

OBJS = Mixer.o
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

all : objs Mixer.so

objs:
	@mkdir -p objs

Mixer.so: $(OBJS)
	@echo Linking...
	@$(CXX) -shared -Xlinker -soname=Mixer.so $(OBJS) -o Mixer.so -lpyro -lmedia
	@echo Adding resources...
	@rescopy Mixer.so -r *.png 

# ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

dist: all inst clean

inst:
	mkdir -p $(PYRO_BINARY_DESTINATION)/System/extensions/dock
	cp -f Mixer.so $(PYRO_BINARY_DESTINATION)/System/extensions/dock/
	addattrib $(PYRO_BINARY_DESTINATION)/System/extensions/dock/Mixer.so "CheckSum::MD5" `md5sum $(PYRO_BINARY_DESTINATION)/System/extensions/dock/Mixer.so | cut -c1-32`
	sync

clean:
	rm -rf $(OBJDIR)
	rm -f Mixer.so
	sync
