CFLAGS = -c -Wall
LFLAGS =
LIBS = -lpyro
CC = g++

OBJS = main.o HexEditWindow.o HexView.o InnerView.o EditController.o FindDialogue.o
EXE = HexEditor

all: $(OBJS)
	$(CC) $(LFLAGS) $(OBJS) -o $(EXE) $(LIBS)
	strip --strip-all $(EXE)
	rescopy -r $(EXE) resources/*.png
	rescopy -a $(EXE) resources/$(EXE).catalog
	for CATALOG in `ls -1 resources`;do \
		if [ -d "resources/$$CATALOG" ] ; then \
			echo $$CATALOG; \
			rescopy -a $(EXE) $$CATALOG/$(EXE).catalog=resources/$$CATALOG/$(EXE).catalog; \
		fi \
	done;
	addattrib $(EXE) os::Category Development

%.o: %.cpp %.h
	$(CC) -c $< $(CFLAGS) -o $@

# ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

DOCS = COPYING README

dist: catalogs all inst doc web clean

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

doc:
	mkdir -p $(PYRO_BINARY_DESTINATION)Native/Development/Documentation/Development/HexEditor
	for DOC in $(DOCS);do \
		cp -f $$DOC $(PYRO_BINARY_DESTINATION)Native/Development/Documentation/Development/HexEditor/; \
	done;
	for FILES in `find $(PYRO_BINARY_DESTINATION)Native/Development/Documentation/Development/HexEditor/`; do \
		if test -f $$FILES; then \
			addattrib $$FILES "os::MimeType" "text/plain"; \
			addattrib $$FILES "CheckSum::MD5" `md5sum $$FILES | cut -c1-32`; \
		fi; \
	done;
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
	rm -f $(OBJS) $(EXE)
	sync

catalog-clean:
	rm -f resources/$(EXE).catalog resources/$(EXE).h
	for CATALOG in `ls -1 resources`;do \
		rm -f resources/$$CATALOG/$(EXE).catalog; \
	done;
	sync
