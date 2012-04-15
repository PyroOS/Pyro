CFLAGS   += -c -fexceptions -ffast-math
CXXFLAGS += -c -fexceptions -ffast-math
AOPTS   = -c
VPATH = ./
OBJDIR	:= objs
OBJS = ColdFishRemote.o

all :	$(OBJDIR) catalogs $(OBJS) $(OBJDIR)/ColdFishRemote.so

$(OBJDIR):
	mkdir $(OBJDIR)

$(OBJDIR)/ColdFishRemote.so: $(OBJS)
	$(CXX) -shared -Xlinker "-soname=ColdFishRemote.so" $(OBJS) -lpyro -o $(OBJDIR)/ColdFishRemote.so
	rescopy -r $(OBJDIR)/ColdFishRemote.so images/*.png
	rescopy -a ColdFishRemote.so resources/ColdFishRemote.catalog
	for CATALOG in `ls -1 resources`;do \
		if [ -d "resources/$$CATALOG" ] ; then \
			echo $$CATALOG; \
			rescopy -a ColdFishRemote.so $$CATALOG/ColdFishRemote.catalog=resources/$$CATALOG/ColdFishRemote.catalog; \
		fi \
	done;

deps: $(OBJDIR)

-include $(OBJDIR)/*.d

# ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

dist: catalogs all inst clean

catalogs:
	cd resources; \
	catcomp -c ColdFishRemote.cd; \
	for CATALOG in `ls -1`;do \
		if [ -d "$$CATALOG" ] ; then \
			echo $$CATALOG; \
			cd $$CATALOG; \
			catcomp -t ColdFishRemote.cd; \
			cd ..; \
		fi \
	done;
	sync

inst:
	mkdir -p $(PYRO_BINARY_DESTINATION)/System/extensions/dock
	cp -f $(OBJDIR)/ColdFishRemote.so $(PYRO_BINARY_DESTINATION)/System/extensions/dock/
	addattrib $(PYRO_BINARY_DESTINATION)/System/extensions/dock/ColdFishRemote.so "CheckSum::MD5" `md5sum $(PYRO_BINARY_DESTINATION)/System/extensions/dock/ColdFishRemote.so | cut -c1-32`
	sync

clean: catalog-clean
	rm -rf $(OBJDIR)
	rm -f $(OBJS)
	sync

catalog-clean:
	rm -f resources/ColdFishRemote.catalog resources/ColdFishRemote.h
	for CATALOG in `ls -1 resources`;do \
		rm -f resources/$$CATALOG/ColdFishRemote.catalog; \
	done;
	sync
