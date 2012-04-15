CFLAGS   += -c -fexceptions -ffast-math
CXXFLAGS += -c -fexceptions -ffast-math
AOPTS    = -c

VPATH = ./
OBJS	= raw_codec.o
OBJDIR	:= objs

include $(PYRO_COMMON)/makefile.rules

all :	$(OBJDIR) $(OBJS) $(OBJDIR)/raw.so

$(OBJDIR):
	mkdir $(OBJDIR)

$(OBJDIR)/raw.so: $(OBJS)
	$(CXX) -shared -Xlinker "-soname=raw.so" $(OBJS) -lpyro -lmedia -o $(OBJDIR)/raw.so

deps: $(OBJDIR) $(DEPS)

# ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

dist: all inst clean

inst:
	mkdir -p $(PYRO_BINARY_DESTINATION)/System/extensions/media
	cp -f $(OBJDIR)/raw.so $(PYRO_BINARY_DESTINATION)/System/extensions/media/
	addattrib $(PYRO_BINARY_DESTINATION)/System/extensions/media/raw.so "CheckSum::MD5" `md5sum $(PYRO_BINARY_DESTINATION)/System/extensions/media/raw.so | cut -c1-32`
	sync

clean:
	rm -rf $(OBJDIR)
	sync
