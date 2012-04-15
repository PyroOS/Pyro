INCLUDES = -I../include/JavaScriptCore -I../include/JavaScriptCore/bindings -I../include/JavaScriptCore/wtf -I../include/WebCore -I../include/WebCore/dom -I../include/WebCore/platform -I../include/WebCore/page -I../include/WebCore/platform/graphics -I../include/WebCore/platform/graphics/svg -I../include/WebCore/editing -I../include/WebCore/xml \
			-I../include/WebCore/rendering -I../include/WebCore/loader -I../include/WebCore/loader/icon -I../include/WebCore/loader/syllable -I../include/WebCore/ss -I../include/WebCore/platform/network -I../include/WebCore/platform/syllable -I../include/WebCore/bindings/js -I../include/JavaScriptCore/kjs -I../include/WebCore/html \
			-I../include/WebCore/bridge -I../include/WebCore/DerivedSources/WebCore -I../include/WebCore/platform/text -I../include/WebCore/platform/sql -I../include/WebCore/platform/image-decoders -I../include/WebCore/platform/image-decoders/gif -I../include/WebCore/platform/image-decoders/bmp \
			-I../include/WebCore/platform/image-decoders/ico -I../include/WebCore/platform/image-decoders/jpeg -I../include/WebCore/platform/image-decoders/png \
			-I../include/WebCore/platform/image-decoders/xbm -I../include/WebCore/ForwardingHeaders -I../include/WebCore/css -I../include/WebCore/history \
			-I../include/WebCore/platform/network -I../include/WebCore/platform/network/syllable -Iinclude \

CFLAGS   += -O2 -c -fexceptions $(INCLUDES) -DBUILDING_SYLLABLE__=1 -DUSE_SYSTEM_MALLOC=1 -D_CRTDBG_MAP_ALLOC -DHAVE_CONFIG_H=1
CXXFLAGS += -O2 -c -fexceptions $(INCLUDES) -DBUILDING_SYLLABLE__=1 -DUSE_SYSTEM_MALLOC=1 -D_CRTDBG_MAP_ALLOC -DHAVE_CONFIG_H=1

OBJS  = webview frameloaderclient chromeclient websettings downloaddialog
OBJS := $(addsuffix .o,$(OBJS))

OBJDIR = objs

VER  = 1
LIB  = libwebview.so

include $(PYRO_COMMON)/makefile.rules

all: $(OBJDIR) $(LIB)

$(OBJDIR):
	mkdir $(OBJDIR)

$(LIB): $(OBJS)
	$(CXX) -shared -Wl,-soname=$(LIB).$(VER) -L. -L../include/JavaScriptCore/objs/ -ljavascriptcore -L../include/WebCore/objs/ -lwebcore -lpyro $^ -o $(LIB).$(VER)

deps: $(OBJDIR) $(DEPS)

-include $(OBJDIR)/%.d

# ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

dist: all inst web clean

inst:
	mkdir -p $(PYRO_BINARY_DESTINATION)/System/libraries
	cp -pf $(LIB).$(VER) $(PYRO_BINARY_DESTINATION)/System/libraries/
	ln -sf $(LIB).$(VER) $(PYRO_BINARY_DESTINATION)/System/libraries/$(LIB)
	addattrib $(PYRO_BINARY_DESTINATION)/System/libraries/$(LIB).$(VER) "CheckSum::MD5" `md5sum $(PYRO_BINARY_DESTINATION)/System/libraries/$(LIB).$(VER) | cut -c1-32`
	addattrib $(PYRO_BINARY_DESTINATION)/System/libraries/$(LIB).$(VER) "os::Icon" "/System/icons/logo.png"
	addattrib $(PYRO_BINARY_DESTINATION)/System/libraries/$(LIB).$(VER) "os::MimeType" "pyro/library"
	mkdir -p $(PYRO_BINARY_DESTINATION)Native/Development/Include
	cp -rf include/* $(PYRO_BINARY_DESTINATION)Native/Development/Include/
	sync

web:
	mkdir -p $(PYRO_BINARY_DESTINATION)Web/Languages/LIST
	mkdir -p $(PYRO_BINARY_DESTINATION)Web/Languages/Libraries/libwebview
	sync

clean:
	rm -rf $(OBJS) $(OBJDIR) $(LIB).$(VER)
	sync
