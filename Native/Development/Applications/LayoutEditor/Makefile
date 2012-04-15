# Makefile for LayoutEditor

COPTS = -Wall -c -fexceptions -O2

OBJS = AppWindow.o EditList.o EditString.o PropertyRow.o WidgetsControls.o WidgetsImage.o WidgetsList.o WidgetsMenu.o WidgetsNodes.o WidgetsUtils.o application.o main.o mainwindow.o ccatalog.o
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

all : translations objs "LayoutEditor"

translations:

objs:
	@mkdir -p objs

"LayoutEditor": $(OBJS)
	@echo Linking...
	@$(CXX) $(OBJS) -o "LayoutEditor" -lpyro
	@echo Adding resources...
	@rescopy "LayoutEditor" -r images/breaker.png images/open.png images/save.png icon24x24.png icon48x48.png application_interface.png 
	strip --strip-all LayoutEditor
	@addattrib "LayoutEditor" os::Category Development

# ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

DOCS = Bugs.txt Widgets.txt

dist: all inst doc web clean

inst:
	mkdir -p $(PYRO_BINARY_DESTINATION)Native/Development/Applications/Development
	cp -f LayoutEditor $(PYRO_BINARY_DESTINATION)Native/Development/Applications/Development/
	addattrib $(PYRO_BINARY_DESTINATION)Native/Development/Applications/Development/LayoutEditor "CheckSum::MD5" `md5sum $(PYRO_BINARY_DESTINATION)Native/Development/Applications/Development/LayoutEditor | cut -c1-32`
	sync

doc:
	mkdir -p $(PYRO_BINARY_DESTINATION)Native/Development/Documentation/Development/LayoutEditor
	for DOC in $(DOCS);do \
		cp -f $$DOC $(PYRO_BINARY_DESTINATION)Native/Development/Documentation/Development/LayoutEditor/; \
	done;
	for FILES in `find $(PYRO_BINARY_DESTINATION)Native/Development/Documentation/Development/LayoutEditor/`; do \
		if test -f $$FILES; then \
			addattrib $$FILES "os::MimeType" "text/plain"; \
			addattrib $$FILES "CheckSum::MD5" `md5sum $$FILES | cut -c1-32`; \
		fi; \
	done;
	sync

web:
	mkdir -p $(PYRO_BINARY_DESTINATION)Web/Languages/LIST
	mkdir -p $(PYRO_BINARY_DESTINATION)Web/Languages/Development/LayoutEditor
	sync

clean:
	rm -rf $(OBJDIR)
	rm -f "LayoutEditor"
	sync
