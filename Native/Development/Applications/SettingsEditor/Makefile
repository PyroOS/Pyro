CXXFLAGS += -I. -c

OBJDIR = objs
OBJS = app editstring editwin mainwin msgtotext strreq

EXE = SettingsEditor

OBJS := $(addprefix $(OBJDIR)/,$(addsuffix .o,$(OBJS)))

all: $(OBJDIR) $(OBJS) 
	$(CXX) $(OBJS) -lpyro -o $(EXE)
	strip --strip-all $(EXE)
	rescopy -r $(EXE) images/*.png
	rescopy $(EXE) -a resources/$(EXE).catalog
	for CATALOG in `ls -1 resources`;do \
		if [ -d "resources/$$CATALOG" ] ; then \
			echo $$CATALOG; \
			rescopy -a $(EXE) $$CATALOG/$(EXE).catalog=resources/$$CATALOG/$(EXE).catalog; \
		fi \
	done;
	addattrib $(EXE) os::Category Development

$(OBJDIR):
	mkdir -p $(OBJDIR)

$(OBJDIR)/%.o : %.cpp
	@echo Compiling : $<
	@$(CXX) $(CXXFLAGS) $< -o $@

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
	mkdir -p $(PYRO_BINARY_DESTINATION)Native/Development/Applications/Development
	cp -f $(EXE) $(PYRO_BINARY_DESTINATION)Native/Development/Applications/Development/
	addattrib $(PYRO_BINARY_DESTINATION)Native/Development/Applications/Development/$(EXE) "CheckSum::MD5" `md5sum $(PYRO_BINARY_DESTINATION)Native/Development/Applications/Development/$(EXE) | cut -c1-32`
	sync

web:
	mkdir -p $(PYRO_BINARY_DESTINATION)Web/Languages/LIST
	mkdir -p $(PYRO_BINARY_DESTINATION)Web/Languages/Development/$(EXE)
	cd resources; \
	for CATALOG in `ls -1`;do \
		if [ -d "$$CATALOG" ] ; then \
			touch $(PYRO_BINARY_DESTINATION)Web/Languages/LIST/$$CATALOG; \
			touch $(PYRO_BINARY_DESTINATION)Web/Languages/Development/$(EXE)/int; \
			touch $(PYRO_BINARY_DESTINATION)Web/Languages/Development/$(EXE)/$$CATALOG; \
		fi \
	done;
	sync

clean: catalog-clean
	rm -f $(OBJS)
	rm -rf $(OBJDIR)
	rm -f $(EXE)
	sync

catalog-clean:
	rm -f resources/$(EXE).catalog resources/$(EXE).h
	for CATALOG in `ls -1 resources`;do \
		rm -f resources/$$CATALOG/$(EXE).catalog; \
	done;
	sync
