CFLAGS   += -c -fexceptions -ffast-math
CXXFLAGS += -c -fexceptions -ffast-math
AOPTS   = -c

VPATH = ./

EXE = ColdFish

OBJS = lcd.o SelectWin.o ColdFish.o cimagebutton.o

OBJDIR	= objs/

include $(PYRO_COMMON)/makefile.rules

all :	$(OBJDIR) catalogs $(OBJS) $(OBJDIR)/ColdFish

$(OBJDIR):
	mkdir $(OBJDIR)

$(OBJDIR)/ColdFish: $(OBJS)
	$(CXX) $(OBJS) -lpyro -lmedia -o ColdFish
	rescopy -r ColdFish images/*.png
	rescopy -a $(EXE) resources/$(EXE).catalog
	for CATALOG in `ls -1 resources`;do \
		if [ -d "resources/$$CATALOG" ] ; then \
			echo $$CATALOG; \
			rescopy -a $(EXE) $$CATALOG/$(EXE).catalog=resources/$$CATALOG/$(EXE).catalog; \
		fi \
	done;
	addattrib ColdFish os::Category Media

deps: $(OBJDIR) $(DEPS)

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
	mkdir -p $(PYRO_BINARY_DESTINATION)/Applications/ColdFish
	cp -pf ColdFish $(PYRO_BINARY_DESTINATION)/Applications/ColdFish/
	addattrib $(PYRO_BINARY_DESTINATION)/Applications/ColdFish/ColdFish "CheckSum::MD5" `md5sum $(PYRO_BINARY_DESTINATION)/Applications/ColdFish/ColdFish | cut -c1-32`
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
	rm -rf $(OBJS) $(OBJDIR) ColdFish
	sync

catalog-clean:
	rm -f resources/$(EXE).catalog resources/$(EXE).h
	for CATALOG in `ls -1 resources`;do \
		rm -f resources/$$CATALOG/$(EXE).catalog; \
	done;
	sync
