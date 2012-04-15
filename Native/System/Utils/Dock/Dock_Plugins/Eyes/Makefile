# Makefile for Eyes.so

COPTS = -c -fexceptions -ffast-math

OBJS = eyewidget.o main.o
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

all : objs Eyes.so

objs:
	@mkdir -p objs

Eyes.so: $(OBJS)
	@echo Linking...
	@$(CXX) -shared -Xlinker -soname=Eyes.so $(OBJS) -o Eyes.so -lpyro

# ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

dist: all inst clean

inst:
	mkdir -p $(PYRO_BINARY_DESTINATION)/System/extensions/dock
	cp -f Eyes.so $(PYRO_BINARY_DESTINATION)/System/extensions/dock/
	addattrib $(PYRO_BINARY_DESTINATION)/System/extensions/dock/Eyes.so "CheckSum::MD5" `md5sum $(PYRO_BINARY_DESTINATION)/System/extensions/dock/Eyes.so | cut -c1-32`
	sync

clean:
	rm -rf $(OBJDIR)
	rm -f Eyes.so
	sync
