# Makefile for ported software.

BASE=`pwd`
TARGET = Bastards
URL = http://source.pyro-os.org/
NAME = xpdf
VERSION = 3.00
FTNAME = freetype
FTVERSION = 2.1.5
ARGS =	--with-freetype2-includes=$(BASE)/../$(FTNAME)/include/freetype2 \
		--enable-a4-paper
DOCS = ANNOUNCE CHANGES INSTALL README

dist: freetype build inst doc package web clean

freetype:
	wget -N $(URL)$(FTNAME)-$(FTVERSION).tar.bz2 && sync && \
	tar -xjkvf $(FTNAME)-$(FTVERSION).tar.bz2 && sync && \
	cd $(FTNAME)-$(FTVERSION) && \
	./configure prefix=$(BASE)/../$(FTNAME) --build=i586 && sync && \
	make && make install && sync && \
	cd ..
	sync

build:
	wget -N $(URL)$(NAME)-$(VERSION).tar.bz2 && sync && \
	tar -xjkvf $(NAME)-$(VERSION).tar.bz2 && sync && \
	cd $(NAME)-$(VERSION) && \
	patch --dry-run	--strip=1 < ../PATCH/$(NAME)-pl3.patch && \
	patch --strip=1 < ../PATCH/$(NAME)-pl3.patch && sync && \
	patch --dry-run	--strip=1 < ../PATCH/$(NAME)-pyro.patch && \
	patch --strip=1 < ../PATCH/$(NAME)-pyro.patch && sync && \
	cp -vrf ../PATCH/* ./ && sync && \
	./configure $(ARGS) && sync && \
	make xpdf && sync && \
	cd ..

inst:
	mkdir -p $(PYRO_BINARY_DESTINATION)/Applications/Xpdf
	cp -pf $(NAME)-$(VERSION)/xpdf/Xpdf $(PYRO_BINARY_DESTINATION)/Applications/Xpdf/
	addattrib $(PYRO_BINARY_DESTINATION)/Applications/Xpdf/Xpdf "CheckSum::MD5" `md5sum $(PYRO_BINARY_DESTINATION)/Applications/Xpdf/Xpdf | cut -c1-32` && \
	sync

doc:
	mkdir -p $(PYRO_BINARY_DESTINATION)/Documentation/nix/$(NAME)
	for DOC in $(DOCS);do \
		cp -f $(NAME)-$(VERSION)/$$DOC $(PYRO_BINARY_DESTINATION)/Documentation/nix/$(NAME)/; \
	done;
	for FILES in `find $(PYRO_BINARY_DESTINATION)/Documentation/`; do \
		if test -f $$FILES; then \
			addattrib $$FILES "os::MimeType" "text/plain"; \
		fi; \
	done;
	sync

package:
	mkdir -p $(PYRO_BINARY_DESTINATION)Source/$(TARGET) && \
	cp -f $(NAME)-$(VERSION).tar.bz2 $(PYRO_BINARY_DESTINATION)Source/$(TARGET)/ && \
	cp -f $(FTNAME)-$(FTVERSION).tar.bz2 $(PYRO_BINARY_DESTINATION)Source/$(TARGET)/ && \
	sync

web:
	mkdir -p $(PYRO_BINARY_DESTINATION)Web/Languages/LIST
	mkdir -p $(PYRO_BINARY_DESTINATION)Web/Languages/Applications/$(NAME)
	cd $(NAME)-$(VERSION)/xpdf/resources; \
	for CATALOG in `ls -1`;do \
		if [ -d "$$CATALOG" ] ; then \
			touch $(PYRO_BINARY_DESTINATION)Web/Languages/LIST/$$CATALOG; \
			touch $(PYRO_BINARY_DESTINATION)Web/Languages/Applications/$(NAME)/int; \
			touch $(PYRO_BINARY_DESTINATION)Web/Languages/Applications/$(NAME)/$$CATALOG; \
		fi \
	done;
	sync

clean:
	rm -rf $(FTNAME)
	rm -rf $(NAME)-$(VERSION)
	rm -rf $(FTNAME)-$(FTVERSION)
	rm -f $(NAME)-$(VERSION).tar.bz2
	rm -f $(FTNAME)-$(FTVERSION).tar.bz2
	sync
