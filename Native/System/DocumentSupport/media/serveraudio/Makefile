CFLAGS   += -c -fexceptions
CXXFLAGS += -c -fexceptions
AOPTS    = -c

VPATH = ./
OBJS	= server_output.o
OBJDIR	:= objs

include $(PYRO_COMMON)/makefile.rules

all :	$(OBJDIR) $(OBJS) $(OBJDIR)/serveraudio.so

$(OBJDIR):
	mkdir $(OBJDIR)

$(OBJDIR)/serveraudio.so: $(OBJS)
	$(CXX) -shared -Xlinker "-soname=serveraudio.so" $(OBJS) -lpyro -lmedia -o $(OBJDIR)/serveraudio.so

deps: $(OBJDIR) $(DEPS)

# ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

dist: all inst clean

inst:
	mkdir -p $(PYRO_BINARY_DESTINATION)/System/extensions/media
	cp -f $(OBJDIR)/serveraudio.so $(PYRO_BINARY_DESTINATION)/System/extensions/media/
	addattrib $(PYRO_BINARY_DESTINATION)/System/extensions/media/serveraudio.so "CheckSum::MD5" `md5sum $(PYRO_BINARY_DESTINATION)/System/extensions/media/serveraudio.so | cut -c1-32`
	sync

clean:
	rm -rf $(OBJDIR)
	sync
