# Makefile for Printer.so

COPTS = -c -fexceptions -ffast-math

OBJS = Printer.o
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

all : objs Printer.so

objs:
	@mkdir -p objs

Printer.so: $(OBJS)
	@echo Linking...
	@$(CXX) -shared -Xlinker -soname=Printer.so $(OBJS) -o Printer.so -lpyro
	@echo Adding resources...
	@rescopy Printer.so -r *.png 

# ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

dist: all inst clean

inst:
	mkdir -p $(PYRO_BINARY_DESTINATION)/System/extensions/dock
	cp -f Printer.so $(PYRO_BINARY_DESTINATION)/System/extensions/dock/
	addattrib $(PYRO_BINARY_DESTINATION)/System/extensions/dock/Printer.so "CheckSum::MD5" `md5sum $(PYRO_BINARY_DESTINATION)/System/extensions/dock/Printer.so | cut -c1-32`
	sync

clean:
	rm -rf $(OBJDIR)
	rm -f Printer.so
	sync
