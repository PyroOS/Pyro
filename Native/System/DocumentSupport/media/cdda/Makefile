CFLAGS   += -c -fexceptions -ffast-math
CXXFLAGS += -c -fexceptions -ffast-math
AOPTS    = -c

VPATH = ./
OBJS	= cdda_input.o
OBJDIR	:= objs

include $(PYRO_COMMON)/makefile.rules

all :	$(OBJDIR) $(OBJS) $(OBJDIR)/cdda.so

$(OBJDIR):
	mkdir $(OBJDIR)

$(OBJDIR)/cdda.so: $(OBJS)
	$(CXX) -shared -Xlinker "-soname=cdda.so" $(OBJS) -lpyro -lmedia -lm -o $(OBJDIR)/cdda.so

deps: $(OBJDIR) $(DEPS)

# ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

dist: all inst clean

inst:
	mkdir -p $(PYRO_BINARY_DESTINATION)/System/extensions/media
	cp -f $(OBJDIR)/cdda.so $(PYRO_BINARY_DESTINATION)/System/extensions/media/
	addattrib $(PYRO_BINARY_DESTINATION)/System/extensions/media/cdda.so "CheckSum::MD5" `md5sum $(PYRO_BINARY_DESTINATION)/System/extensions/media/cdda.so | cut -c1-32`
	sync

clean:
	rm -rf $(OBJDIR)
	sync
