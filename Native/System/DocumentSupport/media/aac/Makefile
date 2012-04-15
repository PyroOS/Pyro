CFLAGS   += -c -fexceptions -ffast-math
CXXFLAGS += -c -fexceptions -ffast-math -DUSE_FFMPEG
AOPTS    = -c

VPATH = ./
OBJS	= aac_codec.o
OBJDIR	:= objs

include $(PYRO_COMMON)/makefile.rules

all :	$(OBJDIR) $(OBJS) $(OBJDIR)/aac.so

$(OBJDIR):
	mkdir $(OBJDIR)

$(OBJDIR)/aac.so: $(OBJS)
	$(CXX) -shared -Xlinker "-soname=aac.so" $(OBJS) -lpyro -lmedia -lfaad -o $(OBJDIR)/aac.so

deps: $(OBJDIR) $(DEPS)

# ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

dist: all inst clean

inst:
	mkdir -p $(PYRO_BINARY_DESTINATION)/System/extensions/media
	cp -f $(OBJDIR)/aac.so $(PYRO_BINARY_DESTINATION)/System/extensions/media/
	addattrib $(PYRO_BINARY_DESTINATION)/System/extensions/media/aac.so "CheckSum::MD5" `md5sum $(PYRO_BINARY_DESTINATION)/System/extensions/media/aac.so | cut -c1-32`
	sync

clean:
	rm -rf $(OBJDIR)
	sync
