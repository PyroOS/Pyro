#Makefile for Transferrer
COPTS = -Wall -c -O2 -fexceptions

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

all : objs "Transferrer"

objs:
	@mkdir -p objs

"Transferrer": $(OBJS)
	@echo Linking...
	@$(CXX) $(OBJS) -o "Transferrer" -L. -lpyro -lstdc++ -lcurl
	@echo Adding resources...
	@rescopy "Transferrer" -r ./images/*.png 
	@addattrib "Transferrer" os::Category Internet

# ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

dist: all inst web clean

inst:
	mkdir -p $(PYRO_BINARY_DESTINATION)/Applications/Transferrer
	cp -f Transferrer $(PYRO_BINARY_DESTINATION)/Applications/Transferrer/
	addattrib $(PYRO_BINARY_DESTINATION)/Applications/Transferrer/Transferrer "CheckSum::MD5" `md5sum $(PYRO_BINARY_DESTINATION)/Applications/Transferrer/Transferrer | cut -c1-32`
	sync

web:
	mkdir -p $(PYRO_BINARY_DESTINATION)Web/Languages/LIST
	mkdir -p $(PYRO_BINARY_DESTINATION)Web/Languages/Applications/Transferrer
	sync

clean:
	rm -rf $(OBJDIR)
	rm -f Transferrer
	sync
