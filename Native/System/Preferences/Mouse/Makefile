# Makefile for Mouse Preference

CXXFLAGS += -c -fexceptions
LIBS = -lpyro

OBJS = 	main.o appwindow.o

OBJDIR := objs
OBJS	:= $(addprefix $(OBJDIR)/,$(OBJS))

EXE = Mouse

# Rules
$(OBJDIR)/%.o : %.cpp
	@echo Compiling : $<
	@$(CXX) $(CXXFLAGS) $< -o $@

all : objs mouse

objs:
	@echo Creating subdirectories...
	@mkdir -p objs

mouse: $(OBJS)
	@echo Linking...
	@$(CXX) $(LFLAGS) $(LIBS) $(OBJS) -o $(EXE)
	@strip --strip-all $(EXE)
	@echo Adding resources...
	@rescopy -r $(EXE) images/icon*.png
	rescopy -a $(EXE) resources/mouse.catalog
	for CATALOG in `ls -1 resources`;do \
		if [ -d "resources/$$CATALOG" ] ; then \
			echo $$CATALOG; \
			rescopy -a $(EXE) $$CATALOG/mouse.catalog=resources/$$CATALOG/mouse.catalog; \
		fi \
	done;

# ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

dist: catalogs all inst web clean

catalogs:
	cd resources; \
	catcomp -c mouse.cd; \
	for CATALOG in `ls -1`;do \
		if [ -d "$$CATALOG" ] ; then \
			echo $$CATALOG; \
			cd $$CATALOG; \
			catcomp -t mouse.cd; \
			cd ..; \
		fi \
	done;
	sync

inst:
	mkdir -p $(PYRO_BINARY_DESTINATION)/Preferences
	cp -pf $(EXE) $(PYRO_BINARY_DESTINATION)/Preferences/
	addattrib $(PYRO_BINARY_DESTINATION)/Preferences/$(EXE) "CheckSum::MD5" `md5sum $(PYRO_BINARY_DESTINATION)/Preferences/$(EXE) | cut -c1-32`
	sync

web:
	mkdir -p $(PYRO_BINARY_DESTINATION)Web/Languages/LIST
	mkdir -p $(PYRO_BINARY_DESTINATION)Web/Languages/Preferences/$(EXE)
	cd resources; \
	for CATALOG in `ls -1`;do \
		if [ -d "$$CATALOG" ] ; then \
			touch $(PYRO_BINARY_DESTINATION)Web/Languages/LIST/$$CATALOG; \
			touch $(PYRO_BINARY_DESTINATION)Web/Languages/Preferences/$(EXE)/int; \
			touch $(PYRO_BINARY_DESTINATION)Web/Languages/Preferences/$(EXE)/$$CATALOG; \
		fi \
	done;
	sync

clean: catalog-clean
	rm -rf $(OBJDIR)
	rm -f $(EXE)
	sync

catalog-clean:
	rm -f resources/mouse.catalog resources/mouse.h
	for CATALOG in `ls -1 resources`;do \
		rm -f resources/$$CATALOG/mouse.catalog; \
	done;
	sync
