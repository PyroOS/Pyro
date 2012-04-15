CXXFLAGS += -c
LIBS = -lpyro

OBJS = main.o Hostname.o Nameserver.o Hosts.o App.o MainWin.o ListViewEditRow.o StringWriter.o StringReader.o Interface.o IFaceWin.o

EXE = Network

all: $(OBJS)
	$(CXX) $(LFLAGS) $(OBJS) -o $(EXE) $(LIBS)
	strip --strip-all $(EXE)
	rescopy $(EXE) -r resources/*.png
	rescopy -a $(EXE) resources/$(EXE).catalog
	for CATALOG in `ls -1 resources`;do \
		if [ -d "resources/$$CATALOG" ] ; then \
			echo $$CATALOG; \
			rescopy -a $(EXE) $$CATALOG/$(EXE).catalog=resources/$$CATALOG/$(EXE).catalog; \
		fi \
	done;

%.o: %.cpp %.h
	$(CXX) -c $< $(CXXFLAGS) -o $@

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
	rm -f $(OBJS)
	rm -f $(EXE)
	sync

catalog-clean:
	rm -f resources/$(EXE).catalog resources/$(EXE).h
	for CATALOG in `ls -1 resources`;do \
		rm -f resources/$$CATALOG/$(EXE).catalog; \
	done;
	sync
