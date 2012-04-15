#Makefile for SBasic
COPTS = -c -fexceptions -ffast-math

APPBIN = /system/bin
OBJS = main.o appwindow.o commthread.o


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

all : objs loop

objs:
	@mkdir -p objs

loop: $(OBJS)
	@echo Linking...
	@$(CXX) $(OBJS) -o loop -lsyllable

clean:
	@echo Cleaning...
	@rm -f $(OBJDIR)/*
	@rm -f loop

