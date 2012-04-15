# Makefile for ported software.

URL = http://builds.nightly.webkit.org/files/trunk/src/
NAME = WebKit
VERSION = r29946

dist: prepare build inst package clean

prepare:
	wget -N $(URL)$(NAME)-$(VERSION).tar.bz2 && sync && \
	tar -xjkvf $(NAME)-$(VERSION).tar.bz2 && sync && \
	cd $(NAME)-$(VERSION)/JavaScriptCore && \
	patch --dry-run	--strip=1 < ../../PATCH/JavaScriptCore.patch && \
	patch --strip=1 < ../../PATCH/JavaScriptCore.patch && sync && \
	cp -vrf ../../PATCH/JavaScriptCore/* ./ && sync && \
	cd ../WebCore && \
	patch --dry-run	--strip=1 < ../../PATCH/WebCore.patch && \
	patch --strip=1 < ../../PATCH/WebCore.patch && sync && \
	cp -vrf ../../PATCH/WebCore/* ./ && sync && \
	cd ../.. && \
	sync

build:
	ln -sf $(NAME)-$(VERSION) include
	cp -vf PATCH/compile ./
	./compile "$(MAKE) -C ./$(NAME)-$(VERSION)/JavaScriptCore -f Makefile.pyro"
	./compile "$(MAKE) -C ./$(NAME)-$(VERSION)/WebCore -f Makefile.pyro"
	$(MAKE) -C WebView dist
	$(MAKE) -C Webster dist
	sync

inst:
	mkdir -p $(PYRO_BINARY_DESTINATION)/System/nix/lib
	mkdir -p $(PYRO_BINARY_DESTINATION)Native/Development/Include/webkit
	cp -pf $(NAME)-$(VERSION)/JavaScriptCore/objs/libjavascriptcore.so.1 $(PYRO_BINARY_DESTINATION)/System/nix/lib/
	cp -pf $(NAME)-$(VERSION)/WebCore/objs/libwebcore.so.1 $(PYRO_BINARY_DESTINATION)/System/nix/lib/
	ln -sf libjavascriptcore.so.1 $(PYRO_BINARY_DESTINATION)/System/nix/lib/libjavascriptcore.so
	ln -sf libwebcore.so.1 $(PYRO_BINARY_DESTINATION)/System/nix/lib/libwebcore.so
	addattrib $(PYRO_BINARY_DESTINATION)/System/nix/lib/libjavascriptcore.so.1 "CheckSum::MD5" `md5sum $(PYRO_BINARY_DESTINATION)/System/nix/lib/libjavascriptcore.so.1 | cut -c1-32`
	addattrib $(PYRO_BINARY_DESTINATION)/System/nix/lib/libwebcore.so.1 "CheckSum::MD5" `md5sum $(PYRO_BINARY_DESTINATION)/System/nix/lib/libwebcore.so.1 | cut -c1-32`
	find $(NAME)-$(VERSION)/JavaScriptCore -depth -name *.h -exec cp {} $(PYRO_BINARY_DESTINATION)Native/Development/Include/webkit/ \; -print
	find $(NAME)-$(VERSION)/WebCore -depth -name *.h -exec cp {} $(PYRO_BINARY_DESTINATION)Native/Development/Include/webkit/ \; -print
	sync

package:
	mkdir -p $(PYRO_BINARY_DESTINATION)Source/$(TARGET) && \
	cp -f $(NAME)-$(VERSION).tar.bz2 $(PYRO_BINARY_DESTINATION)Source/$(TARGET)/ && \
	sync

clean:
	rm -rf $(NAME)-$(VERSION)
	rm -f $(NAME)-$(VERSION).tar.bz2
	rm -f include
	rm -f compile
	sync
