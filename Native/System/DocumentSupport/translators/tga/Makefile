OBJS = tgatrans.o
OBJDIR = objs

#include $(PYRO_COMMON)/makefile.rules

all :	$(OBJDIR) $(OBJDIR)/tgatrans.so

$(OBJDIR):
	mkdir $(OBJDIR)

$(OBJDIR)/tgatrans.so: $(OBJS)
	$(CXX) $(CXXFLAGS) -shared $(OBJS) -lpyro -o $(OBJDIR)/tgatrans.so

# ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

dist: all inst clean

inst:
	mkdir -p $(PYRO_BINARY_DESTINATION)/System/extensions/translators
	cp -f $(OBJDIR)/tgatrans.so $(PYRO_BINARY_DESTINATION)/System/extensions/translators/
	addattrib $(PYRO_BINARY_DESTINATION)/System/extensions/translators/tgatrans.so "CheckSum::MD5" `md5sum $(PYRO_BINARY_DESTINATION)/System/extensions/translators/tgatrans.so | cut -c1-32`
	sync

clean:
	rm -f $(OBJS)
	rm -rf $(OBJDIR)
	sync
