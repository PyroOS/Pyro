#Makefile for Transferrer
COPTS = -Wall -c -O2 -fexceptions

APPBIN = /Applications/Transferrer
#OBJS = main.o application.o mainwindow.o containerview.o localview.o remoteview.o connectbar.o transferthread.o server.o job.o ftpparse.o address_field.o address_field_button.o settingsmenu.o progresswindow.o requesters.o
OBJS = main.o application.o containerview.o localview.o remoteview.o connectbar.o ftpparse.o address_field.o address_field_button.o settingsmenu.o progresswindow.o requesters.o transferthread.o server.o mainwindow.o job.o

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

all : translations objs "Transferrer"


translations:

objs:
	@mkdir -p objs

"Transferrer": $(OBJS)
	@echo Linking...
	@$(CXX) $(OBJS) -o "Transferrer" -L. -lsyllable -lstdc++ -lcurl
	@echo Adding resources...
	@rescopy "Transferrer" -r ./images/*.png 
	@addattrib "Transferrer" os::Category Other

clean:
	@echo Cleaning...
	@rm -f $(OBJDIR)/*
	@rm -f "Transferrer"

deps:

install: all
	@echo Installing...
	@mkdir -p $(IMAGE)/Applications/Transferrer
	@cp "Transferrer" "$(IMAGE)/Applications/Transferrer/Transferrer"

dist: install

