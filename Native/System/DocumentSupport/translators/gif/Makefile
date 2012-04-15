OBJS = giftrans.o
OBJDIR = objs

#include $(PYRO_COMMON)/makefile.rules

all :	$(OBJDIR) $(OBJDIR)/giftrans.so

$(OBJDIR):
	mkdir $(OBJDIR)

$(OBJDIR)/giftrans.so: $(OBJS)
	$(CXX) $(CXXFLAGS) -shared $(OBJS) -lpyro -lgif -o $(OBJDIR)/giftrans.so

# ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

dist: all inst clean

inst:
	mkdir -p $(PYRO_BINARY_DESTINATION)/System/extensions/translators
	cp -f $(OBJDIR)/giftrans.so $(PYRO_BINARY_DESTINATION)/System/extensions/translators/
	addattrib $(PYRO_BINARY_DESTINATION)/System/extensions/translators/giftrans.so "CheckSum::MD5" `md5sum $(PYRO_BINARY_DESTINATION)/System/extensions/translators/giftrans.so | cut -c1-32`
	sync

clean:
	rm -f $(OBJS)
	rm -rf $(OBJDIR)
	sync
