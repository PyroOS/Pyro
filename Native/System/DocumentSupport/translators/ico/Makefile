ICONOBJS = icontrans.o
WINICONOBJS = winicontrans.o
OBJDIR = objs

#include $(PYRO_COMMON)/makefile.rules

all :	$(OBJDIR) $(OBJDIR)/icontrans.so $(OBJDIR)/winicontrans.so

$(OBJDIR):
	mkdir $(OBJDIR)

$(OBJDIR)/icontrans.so: $(ICONOBJS)
	$(CXX) $(CXXFLAGS) -shared $(ICONOBJS) -lpyro -o $(OBJDIR)/icontrans.so

$(OBJDIR)/winicontrans.so: $(WINICONOBJS)
	$(CXX) $(CXXFLAGS) -shared $(WINICONOBJS) -lpyro -o $(OBJDIR)/winicontrans.so

# ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

dist: all inst clean

inst:
	mkdir -p $(PYRO_BINARY_DESTINATION)/System/extensions/translators
	cp -f $(OBJDIR)/icontrans.so $(PYRO_BINARY_DESTINATION)/System/extensions/translators/
	cp -f $(OBJDIR)/winicontrans.so $(PYRO_BINARY_DESTINATION)/System/extensions/translators/
	addattrib $(PYRO_BINARY_DESTINATION)/System/extensions/translators/icontrans.so "CheckSum::MD5" `md5sum $(PYRO_BINARY_DESTINATION)/System/extensions/translators/icontrans.so | cut -c1-32`
	addattrib $(PYRO_BINARY_DESTINATION)/System/extensions/translators/winicontrans.so "CheckSum::MD5" `md5sum $(PYRO_BINARY_DESTINATION)/System/extensions/translators/winicontrans.so | cut -c1-32`
	sync

clean:
	rm -f *.o
	rm -rf $(OBJDIR)
	sync
