CFLAGS   += -c -fexceptions -ffast-math -fno-PIC
CXXFLAGS += -c -fexceptions -ffast-math -fno-PIC
AOPTS    = -c

VPATH = ./
OBJS	= yuv2rgb.o rgb2rgb.o screen.o
OBJDIR	:= objs

include $(PYRO_COMMON)/makefile.rules

all :	$(OBJDIR) $(OBJS) $(OBJDIR)/screen.so

$(OBJDIR):
	mkdir $(OBJDIR)

$(OBJDIR)/screen.so: $(OBJS)
	$(CXX) -shared -Xlinker "-soname=screen.so" $(OBJS) -lpyro -lmedia -o $(OBJDIR)/screen.so

deps: $(OBJDIR) $(DEPS)

# ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

dist: all inst clean

inst:
	mkdir -p $(PYRO_BINARY_DESTINATION)/System/extensions/media
	cp -f $(OBJDIR)/screen.so $(PYRO_BINARY_DESTINATION)/System/extensions/media/
	addattrib $(PYRO_BINARY_DESTINATION)/System/extensions/media/screen.so "CheckSum::MD5" `md5sum $(PYRO_BINARY_DESTINATION)/System/extensions/media/screen.so | cut -c1-32`
	sync

clean:
	rm -rf $(OBJDIR)
	sync
