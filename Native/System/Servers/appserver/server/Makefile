SCREENSHOT = n

CFLAGS   += -c -fexceptions -ffast-math $(shell freetype-config --cflags)
CXXFLAGS += -c -fexceptions -ffast-math $(shell freetype-config --cflags)
AOPTS    = -c
LIBS     = -lpyro

OBJS =  ddriver.o ddline.o ddmemory.o event.o vesadrv.o server.o input.o			\
	config.o keyboard.o keymap.o defaultkeymap.o			\
	sapplication.o swindow.o layer.o toplayer.o render.o bitmap.o crdtrans.o	\
	sfont.o fontnode.o wndborder.o desktop.o winselect.o sprite.o	\
	clipboard.o windowdecorator.o defaultdecorator.o

ifeq ($(SCREENSHOT),y)
	OBJS += screenshot.o
	LIBS += -lpng -lz
endif

ifeq ($(SCREENSHOT),n)
	CXXFLAGS += -D__NO_SCREENSHOT_SUPPORT__=1
endif

OBJDIR	:= objs

include $(PYRO_COMMON)/makefile.rules

all :	$(OBJDIR) $(OBJDIR)/appserver

$(OBJDIR):
	mkdir $(OBJDIR)

$(OBJDIR)/appserver: $(OBJS)
	$(CXX) $(OBJS) -o $(OBJDIR)/appserver $(shell freetype-config --libs) $(LIBS) -lm
	strip --strip-all $(OBJDIR)/appserver

$(OBJDIR)/mini_appserver: $(OBJS)
	$(CXX) $(OBJS) -o $(OBJDIR)/appserver $(shell freetype-config --libs) $(LIBS) -lm

# ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

dist: all inst clean

inst:
	mkdir -p $(PYRO_BINARY_DESTINATION)/System/servers
	cp -pf $(OBJDIR)/appserver $(PYRO_BINARY_DESTINATION)/System/servers/
	addattrib $(PYRO_BINARY_DESTINATION)/System/servers/appserver "CheckSum::MD5" `md5sum $(PYRO_BINARY_DESTINATION)/System/servers/appserver | cut -c1-32`
	addattrib $(PYRO_BINARY_DESTINATION)/System/servers/appserver "os::Icon" "/System/icons/logo.png"
	addattrib $(PYRO_BINARY_DESTINATION)/System/servers/appserver "os::MimeType" "pyro/server"
	sync

clean:
	rm -rf $(OBJS) $(OBJDIR) $(OBJDIR)/appserver
	sync
