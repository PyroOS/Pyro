OBJS = pngtrans.o
OBJDIR = objs

#include $(PYRO_COMMON)/makefile.rules

all :	$(OBJDIR) $(OBJDIR)/pngtrans.so

$(OBJDIR):
	mkdir $(OBJDIR)

$(OBJDIR)/pngtrans.so: $(OBJS)
	$(CXX) $(CXXFLAGS) -shared $(OBJS) -lpyro -lpng -lz -o $(OBJDIR)/pngtrans.so

# ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

dist: all inst clean

inst:
	mkdir -p $(PYRO_BINARY_DESTINATION)/System/extensions/translators
	cp -f $(OBJDIR)/pngtrans.so $(PYRO_BINARY_DESTINATION)/System/extensions/translators/
	addattrib $(PYRO_BINARY_DESTINATION)/System/extensions/translators/pngtrans.so "CheckSum::MD5" `md5sum $(PYRO_BINARY_DESTINATION)/System/extensions/translators/pngtrans.so | cut -c1-32`
	sync

clean:
	rm -f $(OBJS)
	rm -rf $(OBJDIR)
	sync
