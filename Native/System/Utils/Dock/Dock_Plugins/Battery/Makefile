# Makefile for Battery.so

COPTS = -c -fexceptions -ffast-math -I../include

OBJS = Battery.o
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

all : objs Battery.so

objs:
	@mkdir -p objs

Battery.so: $(OBJS)
	@echo Linking...
	@$(CXX) -shared -Xlinker -soname=Battery.so $(OBJS) -o Battery.so -lpyro -lmedia
	@echo Adding resources...
	@rescopy Battery.so -r *.png 

# ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

dist: all inst clean

inst:
	mkdir -p $(PYRO_BINARY_DESTINATION)/System/extensions/dock
	cp -f Battery.so $(PYRO_BINARY_DESTINATION)/System/extensions/dock/
	addattrib $(PYRO_BINARY_DESTINATION)/System/extensions/dock/Battery.so "CheckSum::MD5" `md5sum $(PYRO_BINARY_DESTINATION)/System/extensions/dock/Battery.so | cut -c1-32`
	sync

clean:
	rm -rf $(OBJDIR)
	rm -f Battery.so
	sync
