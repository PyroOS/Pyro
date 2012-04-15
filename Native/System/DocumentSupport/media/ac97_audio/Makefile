CFLAGS   += -c -fexceptions -I../audio_shared
CXXFLAGS += -c -fexceptions -I../audio_shared
AOPTS    = -c

VPATH = ./ ../audio_shared
OBJS	= ac97.o ac97channel.o
OBJDIR	:= objs

include $(PYRO_COMMON)/makefile.rules

all :	$(OBJDIR) $(OBJS) $(OBJDIR)/ac97_audio.so

$(OBJDIR):
	mkdir $(OBJDIR)

$(OBJDIR)/ac97_audio.so: $(OBJS)
	$(CXX) -shared -Xlinker "-soname=ac97_audio.so" $(OBJS) -lpyro -lmedia -o $(OBJDIR)/ac97_audio.so
	
deps: $(OBJDIR) $(DEPS)

# ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

dist: all inst clean

inst:
	mkdir -p $(PYRO_BINARY_DESTINATION)/System/extensions/media
	cp -f $(OBJDIR)/ac97_audio.so $(PYRO_BINARY_DESTINATION)/System/extensions/media/
	addattrib $(PYRO_BINARY_DESTINATION)/System/extensions/media/ac97_audio.so "CheckSum::MD5" `md5sum $(PYRO_BINARY_DESTINATION)/System/extensions/media/ac97_audio.so | cut -c1-32`
	sync

clean:
	rm -rf $(OBJDIR)
	sync
