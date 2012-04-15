# Makefile for Launch

COPTS = -c -fexceptions -ffast-math

OBJS = main.o application.o commonfuncs.o execute.o mainwindow.o

EXE = Launch

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

all : objs $(EXE)

objs:
	@mkdir -p objs

$(EXE): $(OBJS)
	@echo Linking...
	@$(CXX) $(OBJS) -o $(EXE) -lpyro
	@echo Adding resources...
	@rescopy $(EXE) -r images/*.png
	rescopy -a $(EXE) resources/$(EXE).catalog
	for CATALOG in `ls -1 resources`;do \
		if [ -d "resources/$$CATALOG" ] ; then \
			echo $$CATALOG; \
			rescopy -a $(EXE) $$CATALOG/$(EXE).catalog=resources/$$CATALOG/$(EXE).catalog; \
		fi \
	done;

# ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

dist: catalogs all inst web clean

catalogs:
	cd resources; \
	catcomp -c $(EXE).cd; \
	for CATALOG in `ls -1`;do \
		if [ -d "$$CATALOG" ] ; then \
			echo $$CATALOG; \
			cd $$CATALOG; \
			catcomp -t $(EXE).cd; \
			cd ..; \
		fi \
	done;
	sync

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

clean: catalog-clean
	rm -f $(OBJS)
	rm -f $(EXE)
	rm -rf $(OBJDIR)
	sync

catalog-clean:
	rm -f resources/$(EXE).catalog resources/$(EXE).h
	for CATALOG in `ls -1 resources`;do \
		rm -f resources/$$CATALOG/$(EXE).catalog; \
	done;
	sync
