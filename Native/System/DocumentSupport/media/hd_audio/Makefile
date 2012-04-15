CFLAGS   += -c -fexceptions -I../audio_shared
CXXFLAGS += -c -fexceptions -I../audio_shared
AOPTS    = -c

VPATH = ./ ../audio_shared
OBJS	= hda_channel.o hda.o
OBJDIR	:= objs

include $(PYRO_COMMON)/makefile.rules

all :	$(OBJDIR) $(OBJS) $(OBJDIR)/hd_audio.so

$(OBJDIR):
	mkdir $(OBJDIR)

$(OBJDIR)/hd_audio.so: $(OBJS)
	$(CXX) -shared -Xlinker "-soname=hd_audio.so" $(OBJS) -lpyro -lmedia -o $(OBJDIR)/hd_audio.so

deps: $(OBJDIR) $(DEPS)

# ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

dist: all inst clean

inst:
	mkdir -p $(PYRO_BINARY_DESTINATION)/System/extensions/media
	cp -f $(OBJDIR)/hd_audio.so $(PYRO_BINARY_DESTINATION)/System/extensions/media/
	addattrib $(PYRO_BINARY_DESTINATION)/System/extensions/media/hd_audio.so "CheckSum::MD5" `md5sum $(PYRO_BINARY_DESTINATION)/System/extensions/media/hd_audio.so | cut -c1-32`
	sync

clean:
	rm -rf $(OBJDIR)
	sync
