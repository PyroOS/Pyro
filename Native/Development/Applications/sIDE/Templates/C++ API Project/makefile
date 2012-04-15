#Makefile for App
COPTS = -Wall -c -O2 -fexceptions

APPBIN = /Applications/App
OBJS = main.o application.o mainwindow.o

OBJDIR := objs
OBJS	:= $(addprefix $(OBJDIR)/,$(OBJS))

# Rules
$(OBJDIR)/%.o : %.c
	@echo Compiling : $<
	@$(CC) $(COPTS) $< -o $@

$(OBJDIR)/%.o : %.cpp
	@echo Compiling : $<
	@$(CC) $(COPTS) $< -o $@

$(OBJDIR)/%.o : %.s
	@echo Assembling : $<
	@$(CC) $(COPTS) -x assembler-with-cpp $< -o $@

all : translations objs "App"


translations:

objs:
	@mkdir -p objs

"App": $(OBJS)
	@echo Linking...
	@gcc $(OBJS) -o "App"  -lsyllable -lstdc++
	@echo Adding resources...
	@rescopy "App" -r icon48x48.png 
	@addattrib "App" os::Category Other

clean:
	@echo Cleaning...
	@rm -f $(OBJDIR)/*
	@rm -f "App"

deps:

dist: all
	@echo Distribution...
	@mkdir -p $(DIST_DIR)/Applications/App
	@cp "App" $(DIST_DIR)/Applications/App/"App"

install: all
	@echo Installing...
	@mkdir -p /Applications/App
	@cp "App" /Applications/App/"App"

