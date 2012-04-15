include Makeconf

CXXFLAGS += -I. -Iinclude -Ilibpop3/ -Ilibsmtp/

.PHONY: all catalogs settings libs plugins

all: libs $(EXE) plugins

settings: resources/Whisper.catalog $(OBJDIR)
	make all -C $(SETTINGS)

libs:
	for LIB in $(LIBS);do \
		make static -C $$LIB; \
	done;

plugins:
	for DIR in $(PLUGINS);do \
		make -C $$DIR; \
	done;

.PHONY: install clean catalog-clean subdir-clean

subdir-clean:
	for DIR in $(SETTINGS) $(LIBS) $(PLUGINS);do \
		make clean -C $$DIR; \
	done;

$(EXE) whisper: resources/Whisper.catalog $(OBJDIR) $(OBJS) settings catalogs
	g++ $(OBJS) $(SETTINGS_OBJS) -Llibpop3/ -Llibsmtp/ -lpop3 -lsmtp -lpyro -o $(EXE)
	strip --strip-all $(EXE)
	rescopy -r $(EXE) icons/*.png
	rescopy -a $(EXE) resources/$(EXE).catalog
	for CATALOG in `ls -1 resources`;do \
		if [ -d "resources/$$CATALOG" ] ; then \
			echo $$CATALOG; \
			rescopy -a $(EXE) $$CATALOG/$(EXE).catalog=resources/$$CATALOG/$(EXE).catalog; \
		fi \
	done;
	addattrib $(EXE) "os::Category" "Internet"

$(OBJDIR):
	mkdir -p $(OBJDIR)

$(OBJDIR)/%.o : %.cpp
	@echo Compiling : $<
	@g++ $(CXXFLAGS) $< -o $@

# ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

dist: catalogs all inst checksum web clean

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
	mkdir -p $(PYRO_BINARY_DESTINATION)/Applications/Whisper/plugins/ixport
	cp -pf $(EXE) $(PYRO_BINARY_DESTINATION)/Applications/Whisper/
	cp -pf ixport/dbx/dbx $(PYRO_BINARY_DESTINATION)/Applications/Whisper/plugins/ixport/
	cp -pf ixport/mbox/mbox $(PYRO_BINARY_DESTINATION)/Applications/Whisper/plugins/ixport/
	cp -pf ixport/pst/pst $(PYRO_BINARY_DESTINATION)/Applications/Whisper/plugins/ixport/
	sync

checksum:
	addattrib $(PYRO_BINARY_DESTINATION)/Applications/$(EXE)/$(EXE) "CheckSum::MD5" `md5sum $(PYRO_BINARY_DESTINATION)/Applications/$(EXE)/$(EXE) | cut -c1-32`
	for FILES in `find $(PYRO_BINARY_DESTINATION)/Applications/Whisper/plugins/ixport/`; do \
		if test -f $$FILES; then \
			addattrib $$FILES "CheckSum::MD5" `md5sum $$FILES | cut -c1-32`; \
		fi; \
	done;
	sync

web:
	mkdir -p $(PYRO_BINARY_DESTINATION)Web/Languages/LIST
	mkdir -p $(PYRO_BINARY_DESTINATION)Web/Languages/Applications/Whisper
	cd resources; \
	for CATALOG in `ls -1`;do \
		if [ -d "$$CATALOG" ] ; then \
			touch $(PYRO_BINARY_DESTINATION)Web/Languages/LIST/$$CATALOG; \
			touch $(PYRO_BINARY_DESTINATION)Web/Languages/Applications/Whisper/int; \
			touch $(PYRO_BINARY_DESTINATION)Web/Languages/Applications/Whisper/$$CATALOG; \
		fi \
	done;
	sync

clean: catalog-clean subdir-clean
	rm -f $(EXE)
	rm -rf $(OBJDIR)
	sync

catalog-clean:
	rm -f resources/$(EXE).catalog resources/$(EXE).h
	for CATALOG in `ls -1 resources`;do \
		rm -f resources/$$CATALOG/$(EXE).catalog; \
	done;
	sync
