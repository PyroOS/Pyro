#Makefile for format
COPTS = -Wall -c -O2 -fexceptions

OBJS = main.o application.o mainwindow.o splash.o

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

all : objs "format"

$(OBJDIR)/mainwindow.o: mainwindowLayout.cpp

objs:
	@mkdir -p objs

"format": $(OBJS)
	@echo Linking...
	@$(CXX) $(OBJS) -o "format"  -lpyro
	@echo Adding resources...
	@rescopy "format" -r images/icon48x48.png images/logo.png 
	@addattrib "format" os::Category "System Tools"

# ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

dist: all inst clean

inst:
	mkdir -p $(PYRO_BINARY_DESTINATION)/System/binary
	cp -f format $(PYRO_BINARY_DESTINATION)/System/binary/
	addattrib $(PYRO_BINARY_DESTINATION)/System/binary/format "CheckSum::MD5" `md5sum $(PYRO_BINARY_DESTINATION)/System/binary/format | cut -c1-32`; \
	sync

clean:
	rm -rf $(OBJDIR)
	rm -f format
	sync
