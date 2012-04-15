CFLAGS   += -c -fexceptions
CXXFLAGS += -c -fexceptions
AOPTS    = -c

VPATH = ./

OBJS = mediaserver.o mediacontrols.o barview.o

OBJDIR	:= objs

include $(PYRO_COMMON)/makefile.rules

all :	$(OBJDIR) $(OBJS) $(OBJDIR)/mediaserver

$(OBJDIR):
	mkdir $(OBJDIR)

$(OBJDIR)/mediaserver: $(OBJS)
	$(CXX) $(OBJS) -lpyro -lmedia -o mediaserver
	rescopy mediaserver -r *.png

deps: $(OBJDIR) $(DEPS)

# ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

dist: all inst clean

inst:
	mkdir -p $(PYRO_BINARY_DESTINATION)/System/servers
	cp -pf mediaserver $(PYRO_BINARY_DESTINATION)/System/servers/
	addattrib $(PYRO_BINARY_DESTINATION)/System/servers/mediaserver "CheckSum::MD5" `md5sum $(PYRO_BINARY_DESTINATION)/System/servers/mediaserver | cut -c1-32`
	addattrib $(PYRO_BINARY_DESTINATION)/System/servers/mediaserver "os::Icon" "/System/icons/logo.png"
	addattrib $(PYRO_BINARY_DESTINATION)/System/servers/mediaserver "os::MimeType" "pyro/server"
	sync

clean:
	rm -rf $(OBJS) $(OBJDIR) mediaserver
	sync
