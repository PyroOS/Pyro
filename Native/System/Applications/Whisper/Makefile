include Makeconf

CXXFLAGS += -I. -Iinclude -Ilibpop3/ -Ilibsmtp/

.PHONY: all catalogs settings libs plugins

all: libs $(EXE) plugins

catalogs: resources/Whisper.catalog $(CATALOGS)

resources/Whisper.catalog: resources/Whisper.cd
	catcomp -c resources/Whisper.cd

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

install:
	mkdir -p /Applications/Whisper
	cp -pf $(EXE) /Applications/Whisper/
	addattrib /Applications/Whisper/$(EXE) "os::Category" "Internet"
	for DIR in $(PLUGINS);do \
		make install -C $$DIR; \
	done;
	chmod -R -x /Applications/Whisper/*
	chmod +x /Applications/Whisper/$(EXE)

clean: catalog-clean subdir-clean
	-rm $(OBJS)
	-rm $(EXE)

catalog-clean:
	-rm resources/Whisper.catalog resources/Whisper.h; \
	for CATALOG in $(LANGS);do \
		rm resources/$$CATALOG/Whisper.catalog; \
	done;

subdir-clean:
	for DIR in $(SETTINGS) $(LIBS) $(PLUGINS);do \
		make clean -C $$DIR; \
	done;

$(EXE) whisper: resources/Whisper.catalog $(OBJDIR) $(OBJS) settings catalogs
	g++ $(OBJS) $(SETTINGS_OBJS) -Llibpop3/ -Llibsmtp/ -lpop3 -lsmtp -lsyllable -o $(EXE)
	strip --strip-all $(EXE)
	rescopy -r $(EXE) icons/*.png
	rescopy -a $(EXE) resources/Whisper.catalog
	@for CATALOG in $(LANGS);do \
		echo Adding catalog $$CATALOG; \
		rescopy -a $(EXE) $$CATALOG/Whisper.catalog=resources/$$CATALOG/Whisper.catalog; \
	done;

$(OBJDIR):
	mkdir -p $(OBJDIR)

$(OBJDIR)/%.o : %.cpp
	@echo Compiling : $<
	@g++ $(CXXFLAGS) $< -o $@

resources/%/Whisper.catalog : resources/%/Whisper.cd
	@echo Compiling catalog : $<
	@catcomp -t $< > $@
