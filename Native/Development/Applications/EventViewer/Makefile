#Makefile for Event Viewer
COPTS = -Wall -c -O2 -fexceptions

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

all : objs "Event Viewer"

objs:
	@mkdir -p objs

"Event Viewer": $(OBJS)
	@echo Linking...
	@gcc $(OBJS) -o "Event Viewer" -lpyro -lstdc++
	@echo Adding resources...
	@rescopy "Event Viewer" -r icon48x48.png 
	@addattrib "Event Viewer" os::Category Development

# ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

dist: all inst web clean

inst:
	mkdir -p $(PYRO_BINARY_DESTINATION)Native/Development/Applications/Development
	cp -f "Event Viewer" $(PYRO_BINARY_DESTINATION)Native/Development/Applications/Development/
	addattrib "$(PYRO_BINARY_DESTINATION)Native/Development/Applications/Development/Event Viewer" "CheckSum::MD5" `md5sum "$(PYRO_BINARY_DESTINATION)Native/Development/Applications/Development/Event Viewer" | cut -c1-32`
	sync

web:
	mkdir -p $(PYRO_BINARY_DESTINATION)Web/Languages/LIST
	mkdir -p $(PYRO_BINARY_DESTINATION)Web/Languages/Development/EventViewer
	sync

clean:
	rm -rf $(OBJDIR)
	rm -f "Event Viewer"
	sync
