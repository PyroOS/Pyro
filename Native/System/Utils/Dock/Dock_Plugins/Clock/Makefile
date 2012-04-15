# Makefile for Clock.so

COPTS = -c -fexceptions -ffast-math

OBJS = ColorSelector.o Settings.o Clock.o colorbutton.o
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

all : objs Clock.so

objs:
	@mkdir -p objs

Clock.so: $(OBJS)
	@echo Linking...
	@$(CXX) -shared -Xlinker -soname=Clock.so $(OBJS) -o Clock.so -lpyro
	@echo Adding resources...
	@rescopy Clock.so -r icon48x48.png 

# ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

dist: all inst clean

inst:
	mkdir -p $(PYRO_BINARY_DESTINATION)/System/extensions/dock
	cp -f Clock.so $(PYRO_BINARY_DESTINATION)/System/extensions/dock/
	addattrib $(PYRO_BINARY_DESTINATION)/System/extensions/dock/Clock.so "CheckSum::MD5" `md5sum $(PYRO_BINARY_DESTINATION)/System/extensions/dock/Clock.so | cut -c1-32`
	sync

clean:
	rm -rf $(OBJDIR)
	rm -f Clock.so
	sync
