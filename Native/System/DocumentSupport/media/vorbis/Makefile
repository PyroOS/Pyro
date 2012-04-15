CFLAGS   += -c -fexceptions -ffast-math
CXXFLAGS += -c -fexceptions -ffast-math
AOPTS    = -c

VPATH = ./
OBJS	= vorbis_input.o vorbis_codec.o
OBJDIR	:= objs

include $(PYRO_COMMON)/makefile.rules

all :	$(OBJDIR) $(OBJS) $(OBJDIR)/vorbis.so

$(OBJDIR):
	mkdir $(OBJDIR)

$(OBJDIR)/vorbis.so: $(OBJS)
	$(CXX) -shared -Xlinker "-soname=vorbis.so" $(OBJS) -lpyro -lmedia -lvorbisfile -lvorbis -logg -lm -o $(OBJDIR)/vorbis.so

deps: $(OBJDIR) $(DEPS)

# ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

dist: all inst clean

inst:
	mkdir -p $(PYRO_BINARY_DESTINATION)/System/extensions/media
	cp -f $(OBJDIR)/vorbis.so $(PYRO_BINARY_DESTINATION)/System/extensions/media/
	addattrib $(PYRO_BINARY_DESTINATION)/System/extensions/media/vorbis.so "CheckSum::MD5" `md5sum $(PYRO_BINARY_DESTINATION)/System/extensions/media/vorbis.so | cut -c1-32`
	sync

clean:
	rm -rf $(OBJDIR)
	sync
