CFLAGS   += -c -fexceptions -ffast-math
CXXFLAGS += -c -fexceptions -ffast-math -D__STDC_CONSTANT_MACROS
AOPTS    = -c

VPATH = ./
OBJS	= ffmpeg_demux.o ffmpeg_output.o ffmpeg_codec.o ffmpeg_addon.o
OBJDIR	:= objs

all :	$(OBJDIR) $(OBJS) $(OBJDIR)/ffmpeg.so

$(OBJDIR):
	mkdir $(OBJDIR)

$(OBJDIR)/ffmpeg.so: $(OBJS)
	$(CXX) -shared -Xlinker "-soname=ffmpeg.so" $(OBJS) -lpyro -lmedia -lm -lz -lpthread -lavformat -lavcodec -lavutil -ltheora -logg -o $(OBJDIR)/ffmpeg.so

deps: $(LIBS) $(OBJDIR) $(DEPS)

# ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

dist: all inst clean

inst:
	mkdir -p $(PYRO_BINARY_DESTINATION)/System/extensions/media
	cp -f $(OBJDIR)/ffmpeg.so $(PYRO_BINARY_DESTINATION)/System/extensions/media/
	addattrib $(PYRO_BINARY_DESTINATION)/System/extensions/media/ffmpeg.so "CheckSum::MD5" `md5sum $(PYRO_BINARY_DESTINATION)/System/extensions/media/ffmpeg.so | cut -c1-32`
	sync

clean:
	rm -f $(OBJS)
	rm -rf $(OBJDIR)
	sync
