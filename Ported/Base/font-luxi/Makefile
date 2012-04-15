# Makefile for non-compilable items.

BACK=`pwd`
URL = http://xorg.freedesktop.org/releases/individual/font/
NAME = luxi
DOWNLOADNAME = font-bh-ttf
VERSION = 1.0.3
DESC = A family of typefaces originally designed for the X Window System.
DOCS = ChangeLog INSTALL README

dist: download inst mimetypes doc checksum package clean

download:
	wget -N $(URL)$(DOWNLOADNAME)-$(VERSION).tar.gz && sync && \
	tar -xzkvf $(DOWNLOADNAME)-$(VERSION).tar.gz && sync && \
	sync

inst:
	mv $(DOWNLOADNAME)-$(VERSION)/luximb.ttf  $(DOWNLOADNAME)-$(VERSION)/LuxiMono-Bold.ttf && \
	mv $(DOWNLOADNAME)-$(VERSION)/luximbi.ttf $(DOWNLOADNAME)-$(VERSION)/LuxiMono-BoldOblique.ttf && \
	mv $(DOWNLOADNAME)-$(VERSION)/luximr.ttf  $(DOWNLOADNAME)-$(VERSION)/LuxiMono-Regular.ttf && \
	mv $(DOWNLOADNAME)-$(VERSION)/luximri.ttf $(DOWNLOADNAME)-$(VERSION)/LuxiMono-Oblique.ttf && \
	mv $(DOWNLOADNAME)-$(VERSION)/luxirb.ttf  $(DOWNLOADNAME)-$(VERSION)/LuxiSerif-Bold.ttf && \
	mv $(DOWNLOADNAME)-$(VERSION)/luxirbi.ttf $(DOWNLOADNAME)-$(VERSION)/LuxiSerif-BoldOblique.ttf && \
	mv $(DOWNLOADNAME)-$(VERSION)/luxirr.ttf  $(DOWNLOADNAME)-$(VERSION)/LuxiSerif-Regular.ttf && \
	mv $(DOWNLOADNAME)-$(VERSION)/luxirri.ttf $(DOWNLOADNAME)-$(VERSION)/LuxiSerif-Oblique.ttf && \
	mv $(DOWNLOADNAME)-$(VERSION)/luxisb.ttf  $(DOWNLOADNAME)-$(VERSION)/LuxiSans-Bold.ttf && \
	mv $(DOWNLOADNAME)-$(VERSION)/luxisbi.ttf $(DOWNLOADNAME)-$(VERSION)/LuxiSans-BoldOblique.ttf && \
	mv $(DOWNLOADNAME)-$(VERSION)/luxisr.ttf  $(DOWNLOADNAME)-$(VERSION)/LuxiSans-Regular.ttf && \
	mv $(DOWNLOADNAME)-$(VERSION)/luxisri.ttf $(DOWNLOADNAME)-$(VERSION)/LuxiSans-Oblique.ttf && \
	mkdir -p $(PYRO_BINARY_DESTINATION)Ported/$(TARGET)/$(NAME)-$(VERSION)/System/fonts && sync && \
	cp -f $(DOWNLOADNAME)-$(VERSION)/*.ttf $(PYRO_BINARY_DESTINATION)Ported/$(TARGET)/$(NAME)-$(VERSION)/System/fonts/ && sync && \
	sync

mimetypes:
	for FILES in `find $(PYRO_BINARY_DESTINATION)Ported/$(TARGET)/$(NAME)-$(VERSION)/System/fonts/*`; do \
		if test -f $$FILES; then \
			addattrib $$FILES "os::MimeType" "application/x-font-ttf"; \
		fi; \
	done;
	sync

doc:
	mkdir -p $(PYRO_BINARY_DESTINATION)Ported/$(TARGET)/$(NAME)-$(VERSION)/Documentation/Resources/$(NAME)
	for DOC in $(DOCS);do \
		cp -f $(DOWNLOADNAME)-$(VERSION)/$$DOC $(PYRO_BINARY_DESTINATION)Ported/$(TARGET)/$(NAME)-$(VERSION)/Documentation/Resources/$(NAME)/; \
	done;
	for FILES in `find $(PYRO_BINARY_DESTINATION)Ported/$(TARGET)/$(NAME)-$(VERSION)/Documentation/Resources/$(NAME)/`; do \
		if test -f $$FILES; then \
			addattrib $$FILES "os::MimeType" "text/plain"; \
		fi; \
	done;
	sync

checksum:
	for FILES in `find $(PYRO_BINARY_DESTINATION)Ported/$(TARGET)/$(NAME)-$(VERSION)/`; do \
		if test -f $$FILES; then \
			addattrib $$FILES "CheckSum::MD5" `md5sum $$FILES | cut -c1-32`; \
		fi; \
	done;
	sync

package:
	echo $(DESC) > $(TEMP)/zip_comment && \
	mkdir -p $(PYRO_BINARY_DESTINATION)Source/$(TARGET) && \
	cp -f $(DOWNLOADNAME)-$(VERSION).tar.gz $(PYRO_BINARY_DESTINATION)Source/$(TARGET)/$(NAME)-$(VERSION).tar.gz && \
	cd $(PYRO_BINARY_DESTINATION)Ported/$(TARGET)/$(NAME)-$(VERSION) && \
	zip -z < "$(TEMP)/zip_comment" -ry9 $(PYRO_BINARY_DESTINATION)Ported/$(TARGET)/$(NAME)-$(VERSION).zip ./* && \
	cd $(BACK) && \
	rm -rf $(PYRO_BINARY_DESTINATION)Ported/$(TARGET)/$(NAME)-$(VERSION) && \
	sync

clean:
	rm -rf $(DOWNLOADNAME)-$(VERSION)
	rm -f $(DOWNLOADNAME)-$(VERSION).tar.gz
	sync
