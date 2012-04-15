CFLAGS   += -c -fexceptions -ffast-math
CXXFLAGS += -c -fexceptions -ffast-math
AOPTS   = -c

EXE = MediaPlayer

VPATH = ./

OBJS = seekslider.o cimagebutton.o MediaPlayer.o

OBJDIR	:= objs

include $(PYRO_COMMON)/makefile.rules

all :	$(OBJDIR) catalogs $(OBJS) $(OBJDIR)/MediaPlayer

$(OBJDIR):
	mkdir $(OBJDIR)

$(OBJDIR)/MediaPlayer: $(OBJS)
	$(CXX) $(OBJS) -lpyro -lmedia -o MediaPlayer
	rescopy -r MediaPlayer images/*.png
	rescopy -a $(EXE) resources/mediaplayer.catalog
	for CATALOG in `ls -1 resources`;do \
		if [ -d "resources/$$CATALOG" ] ; then \
			echo $$CATALOG; \
			rescopy -a $(EXE) $$CATALOG/mediaplayer.catalog=resources/$$CATALOG/mediaplayer.catalog; \
		fi \
	done;
	addattrib MediaPlayer os::Category Media

deps: $(OBJDIR) $(DEPS)

# ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

dist: catalogs all inst web clean

catalogs:
	cd resources; \
	catcomp -c mediaplayer.cd; \
	for CATALOG in `ls -1`;do \
		if [ -d "$$CATALOG" ] ; then \
			echo $$CATALOG; \
			cd $$CATALOG; \
			catcomp -t mediaplayer.cd; \
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
	rm -rf $(OBJDIR) MediaPlayer
	sync

catalog-clean:
	rm -f resources/mediaplayer.catalog resources/mediaplayer.h
	for CATALOG in `ls -1 resources`;do \
		rm -f resources/$$CATALOG/mediaplayer.catalog; \
	done;
	sync
