#Makefile for Event Viewer
COPTS = -Wall -c -O2 -fexceptions

APPBIN = /Applications/sIDE
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

all : translations objs "Event Viewer"


translations:

objs:
	@mkdir -p objs

"Event Viewer": $(OBJS)
	@echo Linking...
	@gcc $(OBJS) -o "Event Viewer"  -lsyllable -lstdc++
	@echo Adding resources...
	@rescopy "Event Viewer" -r icon48x48.png 
	@addattrib "Event Viewer" os::Category Development

clean:
	@echo Cleaning...
	@rm -f $(OBJDIR)/*
	@rm -f "Event Viewer"

deps:

dist: all
	@echo Distribution...
	@mkdir -p $(DIST_DIR)/Applications/sIDE
	@cp "Event Viewer" "$(DIST_DIR)/Applications/sIDE/Event Viewer"

install: all
	@echo Installing...
	@mkdir -p /Applications/sIDE
	@cp "Event Viewer" "/Applications/sIDE/Event Viewer"

