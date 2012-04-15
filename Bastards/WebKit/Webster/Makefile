INCLUDES = -I../include/JavaScriptCore -I../include/JavaScriptCore/bindings -I../include/JavaScriptCore/wtf -I../include/WebCore -I../include/WebCore/dom -I../include/WebCore/platform -I../include/WebCore/page -I../include/WebCore/platform/graphics -I../include/WebCore/platform/graphics/svg -I../include/WebCore/editing -I../include/WebCore/xml \
			-I../include/WebCore/rendering -I../include/WebCore/loader -I../include/WebCore/loader/icon -I../include/WebCore/loader/syllable -I../include/WebCore/ss -I../include/WebCore/platform/network -I../include/WebCore/platform/syllable -I../include/WebCore/bindings/js -I../include/JavaScriptCore/kjs -I../include/WebCore/html \
			-I../include/WebCore/bridge -I../include/WebCore/DerivedSources/WebCore -I../include/WebCore/platform/text -I../include/WebCore/platform/sql -I../include/WebCore/platform/image-decoders -I../include/WebCore/platform/image-decoders/gif -I../include/WebCore/platform/image-decoders/bmp \
			-I../include/WebCore/platform/image-decoders/ico -I../include/WebCore/platform/image-decoders/jpeg -I../include/WebCore/platform/image-decoders/png \
			-I../include/WebCore/platform/image-decoders/xbm -I../include/WebCore/ForwardingHeaders -I../include/WebCore/css -I../include/WebCore/history \
			-I../include/WebCore/platform/network -I../include/WebCore/platform/network/syllable -I../WebView/include -I. -I./settings_window \

CFLAGS   += -O2 -c -fexceptions $(INCLUDES) -DBUILDING_SYLLABLE__=1 -DUSE_SYSTEM_MALLOC=1 -D_CRTDBG_MAP_ALLOC -DHAVE_CONFIG_H=1
CXXFLAGS += -O2 -c -fexceptions $(INCLUDES) -DBUILDING_SYLLABLE__=1 -DUSE_SYSTEM_MALLOC=1 -D_CRTDBG_MAP_ALLOC -DHAVE_CONFIG_H=1

OBJS  = main browserwebview browserchromeclient urledit urlfilter bookmarks progresspanel
OBJS := $(addsuffix .o,$(OBJS))

SETTINGS_OBJS  = settings_window icon inputview general_view proxy_view
SETTINGS_OBJS := $(addprefix settings_window/,$(addsuffix .o,$(SETTINGS_OBJS)))

OBJS := $(OBJS) $(SETTINGS_OBJS)

OBJDIR = objs

BIN = Webster

include $(PYRO_COMMON)/makefile.rules

all: $(OBJDIR) $(BIN)

$(OBJDIR):
	mkdir $(OBJDIR) $(OBJDIR)/settings_window

$(BIN): version.h $(OBJS)
	g++ $^ -o $@ -L. -L../include/JavaScriptCore/objs/ -ljavascriptcore -L../include/WebCore/objs/ -lwebcore -L$(PYRO_BINARY_DESTINATION)/System/libraries/ -lwebview -lpyro
	strip --strip-all $(BIN)
	rescopy $(BIN) -r images/*.png

version.h:
	$(shell if [ ! -e version.h ]; then \
		printf "#define WEBSTER_VERSION\t\"%s-development\"\n" `date +%Y%m%d` > version.h; \
	fi)

deps: $(OBJDIR) $(DEPS)

-include $(OBJDIR)/%.d

# ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

dist: all inst web clean

inst:
	mkdir -p $(PYRO_BINARY_DESTINATION)/Applications/Webster
	cp -pf Webster $(PYRO_BINARY_DESTINATION)/Applications/Webster/
	addattrib $(PYRO_BINARY_DESTINATION)/Applications/Webster/Webster "CheckSum::MD5" `md5sum $(PYRO_BINARY_DESTINATION)/Applications/Webster/Webster | cut -c1-32`
	addattrib $(PYRO_BINARY_DESTINATION)/Applications/Webster/Webster "os::Category" "Internet"
	sync

web:
	mkdir -p $(PYRO_BINARY_DESTINATION)Web/Languages/LIST
	mkdir -p $(PYRO_BINARY_DESTINATION)Web/Languages/Applications/Webster
	sync

clean:
	rm -rf $(OBJS) $(OBJDIR) version.h $(BIN)
	sync
