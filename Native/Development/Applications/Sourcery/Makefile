# Makefile for Sourcery

COPTS = -O2 -Wall -c 

OBJS = main.o file.o settings.o splash.o application.o editor.o mainwindow.o aboutbox.o statusbar.o formatset.o commonfuncs.o link.o convertdialog.o fileproperties.o finddialog.o replacedialog.o userinserts.o gotodialog.o browser.o settingsdialog.o execute.o toolsdialog.o colorbutton.o
OBJDIR	:= objs
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

all : objs Sourcery

objs:
	@mkdir -p objs

Sourcery: $(OBJS)
	@echo Linking...
	@$(CXX) $(OBJS) -o Sourcery -lpyro -lm -lcodeview
	@strip --strip-all Sourcery
	@echo Adding resources...
	@rescopy Sourcery -r ./images/*.png ./images/mimetypes/*.png 
	@addattrib Sourcery os::Category Development

# ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

dist: all inst web clean

inst:
	mkdir -p $(PYRO_BINARY_DESTINATION)Native/Development/Applications/Development
	cp -f Sourcery $(PYRO_BINARY_DESTINATION)Native/Development/Applications/Development/
	addattrib $(PYRO_BINARY_DESTINATION)Native/Development/Applications/Development/Sourcery "CheckSum::MD5" `md5sum $(PYRO_BINARY_DESTINATION)Native/Development/Applications/Development/Sourcery | cut -c1-32`
	sync

web:
	mkdir -p $(PYRO_BINARY_DESTINATION)Web/Languages/LIST
	mkdir -p $(PYRO_BINARY_DESTINATION)Web/Languages/Development/Sourcery
	sync

clean:
	rm -rf $(OBJDIR)
	rm -f Sourcery
	sync
