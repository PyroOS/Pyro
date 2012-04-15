CFLAGS   += -c -fexceptions -ffast-math
CXXFLAGS += -c -fexceptions -ffast-math
AOPTS    = -c

VPATH = ./
OBJS	= cdrom.o
OBJDIR	:= objs

include $(PYRO_COMMON)/makefile.rules

all :	$(OBJDIR) $(OBJS) $(OBJDIR)/cdrom.so

$(OBJDIR):
	mkdir $(OBJDIR)

$(OBJDIR)/cdrom.so: $(OBJS)
	$(CXX) -shared -Xlinker "-soname=cdrom.so" $(OBJS) -lpyro -lmedia -o $(OBJDIR)/cdrom.so

deps: $(OBJDIR) $(DEPS)

# ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

dist: all inst clean

inst:
	mkdir -p $(PYRO_BINARY_DESTINATION)/System/extensions/media
	cp -f $(OBJDIR)/cdrom.so $(PYRO_BINARY_DESTINATION)/System/extensions/media/
	addattrib $(PYRO_BINARY_DESTINATION)/System/extensions/media/cdrom.so "CheckSum::MD5" `md5sum $(PYRO_BINARY_DESTINATION)/System/extensions/media/cdrom.so | cut -c1-32`
	sync

clean:
	rm -rf $(OBJDIR)
	sync
