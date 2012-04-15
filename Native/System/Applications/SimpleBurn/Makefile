#Makefile for SimpleBurn
COPTS = -Wall -c -fexceptions

EXE = SimpleBurn

OBJS = main.o application.o mainwindow.o splash.o erasewindow.o

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

all : translations objs "SimpleBurn"

$(OBJDIR)/mainwindow.o: mainwindowLayout.cpp

translations:

objs:
	@mkdir -p objs

"SimpleBurn": $(OBJS)
	@echo Linking...
	@$(CXX) $(OBJS) -o "SimpleBurn" -lpyro -lmedia
	@echo Adding resources...
	@rescopy "SimpleBurn" -r images/add-audio.png images/add-video.png images/burn.png images/icon24x24.png images/icon48x48.png images/menu-audio.png images/menu-backup.png images/menu-data.png images/menu-video.png images/move-down.png images/move-up.png images/remove.png images/menu-burn.png images/logo.png 
	@addattrib "SimpleBurn" os::Category Other

# ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

dist: all inst web clean

inst:
	mkdir -p $(PYRO_BINARY_DESTINATION)/Applications/$(EXE)
	cp -pf $(EXE) $(PYRO_BINARY_DESTINATION)/Applications/$(EXE)/
	addattrib $(PYRO_BINARY_DESTINATION)/Applications/$(EXE)/$(EXE) "CheckSum::MD5" `md5sum $(PYRO_BINARY_DESTINATION)/Applications/$(EXE)/$(EXE) | cut -c1-32`
	sync

web:
	mkdir -p $(PYRO_BINARY_DESTINATION)Web/Languages/LIST
	mkdir -p $(PYRO_BINARY_DESTINATION)Web/Languages/Applications/$(EXE)
	cd resources; \
	for CATALOG in `ls -1`;do \
		if [ -d "$$CATALOG" ] ; then \
			touch $(PYRO_BINARY_DESTINATION)Web/Languages/LIST/$$CATALOG; \
			touch $(PYRO_BINARY_DESTINATION)Web/Languages/Applications/$(EXE)/int; \
			touch $(PYRO_BINARY_DESTINATION)Web/Languages/Applications/$(EXE)/$$CATALOG; \
		fi \
	done;
	sync

clean:
	rm -rf $(OBJDIR)
	rm -f $(EXE)
	sync
