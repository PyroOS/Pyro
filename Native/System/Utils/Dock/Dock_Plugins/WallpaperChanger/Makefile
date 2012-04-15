CFLAGS   += -c -fexceptions -ffast-math
CXXFLAGS += -c -fexceptions -ffast-math
AOPTS   = -c
VPATH = ./
OBJDIR	:= objs
OBJS = WallpaperChanger.o WallpaperChangerLooper.o WallpaperChangerSettings.o bitmapscale.o

all :	$(OBJDIR) $(OBJS) $(OBJDIR)/WallpaperChanger.so

$(OBJDIR):
	mkdir $(OBJDIR)

$(OBJDIR)/WallpaperChanger.so: $(OBJS)
	$(CXX) -shared -Xlinker "-soname=WallpaperChanger.so" $(OBJS) -lpyro -o $(OBJDIR)/WallpaperChanger.so
	rescopy -r $(OBJDIR)/WallpaperChanger.so images/*.png

deps: $(OBJDIR)

-include $(OBJDIR)/*.d

# ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

dist: all inst clean

inst:
	mkdir -p $(PYRO_BINARY_DESTINATION)/System/extensions/dock
	cp -f $(OBJDIR)/WallpaperChanger.so $(PYRO_BINARY_DESTINATION)/System/extensions/dock/
	addattrib $(PYRO_BINARY_DESTINATION)/System/extensions/dock/WallpaperChanger.so "CheckSum::MD5" `md5sum $(PYRO_BINARY_DESTINATION)/System/extensions/dock/WallpaperChanger.so | cut -c1-32`
	sync

clean:
	rm -rf $(OBJDIR)
	rm -f $(OBJS)
	sync
