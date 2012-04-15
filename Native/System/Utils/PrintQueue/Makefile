CXXFLAGS += -I. -c

OBJDIR = objs
EXE = print-queue

OBJS := $(addprefix $(OBJDIR)/,$(addsuffix .o,$(EXE)))

all: $(OBJDIR) $(OBJS) 
	$(CXX) $(OBJS) -lpyro -lcups -o $(EXE)
	strip --strip-all $(EXE)
	rescopy -r $(EXE) icons/*.png
	rescopy -a $(EXE) resources/PrintQueue.catalog
	for CATALOG in `ls -1 resources`;do \
		if [ -d "resources/$$CATALOG" ] ; then \
			echo $$CATALOG; \
			rescopy -a $(EXE) $$CATALOG/PrintQueue.catalog=resources/$$CATALOG/PrintQueue.catalog; \
		fi \
	done;

$(OBJDIR):
	mkdir -p $(OBJDIR)

$(OBJDIR)/%.o : %.cpp
	@echo Compiling : $<
	@$(CXX) $(CXXFLAGS) $< -o $@

# ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

dist: catalogs all inst web clean

catalogs:
	cd resources; \
	catcomp -c PrintQueue.cd; \
	for CATALOG in `ls -1`;do \
		if [ -d "$$CATALOG" ] ; then \
			echo $$CATALOG; \
			cd $$CATALOG; \
			catcomp -t PrintQueue.cd; \
			cd ..; \
		fi \
	done;
	sync

inst:
	mkdir -p $(PYRO_BINARY_DESTINATION)/System/binary
	cp -f $(EXE) $(PYRO_BINARY_DESTINATION)/System/binary/
	addattrib $(PYRO_BINARY_DESTINATION)/System/binary/$(EXE) "CheckSum::MD5" `md5sum $(PYRO_BINARY_DESTINATION)/System/binary/$(EXE) | cut -c1-32`
	sync

web:
	mkdir -p $(PYRO_BINARY_DESTINATION)Web/Languages/LIST
	mkdir -p $(PYRO_BINARY_DESTINATION)Web/Languages/Utils/PrintQueue
	cd resources; \
	for CATALOG in `ls -1`;do \
		if [ -d "$$CATALOG" ] ; then \
			touch $(PYRO_BINARY_DESTINATION)Web/Languages/LIST/$$CATALOG; \
			touch $(PYRO_BINARY_DESTINATION)Web/Languages/Utils/PrintQueue/int; \
			touch $(PYRO_BINARY_DESTINATION)Web/Languages/Utils/PrintQueue/$$CATALOG; \
		fi \
	done;
	sync

clean: catalog-clean
	rm -rf $(OBJDIR)
	rm -f $(EXE)
	sync

catalog-clean:
	rm -f resources/PrintQueue.catalog resources/PrintQueue.h
	for CATALOG in `ls -1 resources`;do \
		rm -f resources/$$CATALOG/PrintQueue.catalog; \
	done;
	sync
