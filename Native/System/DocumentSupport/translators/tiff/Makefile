OBJS = tifftrans.o
OBJDIR = objs

#include $(PYRO_COMMON)/makefile.rules

all :	$(OBJDIR) $(OBJDIR)/tifftrans.so

$(OBJDIR):
	mkdir $(OBJDIR)

$(OBJDIR)/tifftrans.so: $(OBJS)
	$(CXX) $(CXXFLAGS) -shared $(OBJS) -lpyro -ltiff -ljpeg -lz -o $(OBJDIR)/tifftrans.so

# ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

dist: all inst clean

inst:
	mkdir -p $(PYRO_BINARY_DESTINATION)/System/extensions/translators
	cp -f $(OBJDIR)/tifftrans.so $(PYRO_BINARY_DESTINATION)/System/extensions/translators/
	addattrib $(PYRO_BINARY_DESTINATION)/System/extensions/translators/tifftrans.so "CheckSum::MD5" `md5sum $(PYRO_BINARY_DESTINATION)/System/extensions/translators/tifftrans.so | cut -c1-32`
	sync

clean:
	rm -f $(OBJS)
	rm -rf $(OBJDIR)
	sync
