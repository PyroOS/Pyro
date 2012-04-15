# Makefile for ported software.

BACK=`pwd`
URL = http://sourceforge.net/projects/espgs/files/espgs/
NAME = espgs
VERSION = 7.07.1
JPEGURL = http://sourceforge.net/projects/libjpeg/files/libjpeg/
JPEGNAME = jpeg
JPEGVERSION = 6b
ARGS =	--prefix=$(PYRO_BINARY_DESTINATION)Ported/$(TARGET)/$(NAME)-$(VERSION)/NIX/$(NAME)
DESC = ESP Ghostscript is the GNU Ghostscript distribution including new drivers, and support for CUPS.
DOCS = CHANGES INSTALL LICENSE README README*

dist: build doc update checksum package clean

build:
	wget -N $(URL)$(VERSION)/$(NAME)-$(VERSION)-source.tar.gz/download && sync && \
	wget -N $(JPEGURL)$(JPEGVERSION)/$(JPEGNAME)src.v$(JPEGVERSION).tar.gz/download && sync && \
	tar -xzkvf $(NAME)-$(VERSION)-source.tar.gz && sync && \
	cd $(NAME)-$(VERSION) && \
	tar -xzkvf ../$(JPEGNAME)src.v$(JPEGVERSION).tar.gz && sync && \
	mv $(JPEGNAME)-$(JPEGVERSION) jpeg && \
	cp jpeg/jdhuff.c jpeg/jdphuff.c && \
	./configure $(ARGS) && sync && \
	make && make install && sync && \
	cd ..

doc:
	mkdir -p $(PYRO_BINARY_DESTINATION)Ported/$(TARGET)/$(NAME)-$(VERSION)/Documentation/nix/$(NAME)
	for DOC in $(DOCS);do \
		cp -f $(NAME)-$(VERSION)/$$DOC $(PYRO_BINARY_DESTINATION)Ported/$(TARGET)/$(NAME)-$(VERSION)/Documentation/nix/$(NAME)/; \
	done;
	for FILES in `find $(PYRO_BINARY_DESTINATION)Ported/$(TARGET)/$(NAME)-$(VERSION)/Documentation/`; do \
		if test -f $$FILES; then \
			addattrib $$FILES "os::MimeType" "text/plain"; \
		fi; \
	done;
	sync

update:
	for FILES in `find $(PYRO_BINARY_DESTINATION)Ported/$(TARGET)/$(NAME)-$(VERSION)/NIX/`; do \
		if [ `file --brief --mime-type $$FILES | grep -c "text" -` != 0 ]; then \
			sed -i "s,$(PYRO_BINARY_DESTINATION)Ported/$(TARGET)/$(NAME)-$(VERSION),/boot,g" $$FILES; \
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
	cp -f $(NAME)-$(VERSION)-source.tar.gz $(PYRO_BINARY_DESTINATION)Source/$(TARGET)/ && \
	cd $(PYRO_BINARY_DESTINATION)Ported/$(TARGET)/$(NAME)-$(VERSION) && \
	zip -z < "$(TEMP)/zip_comment" -ry9 $(PYRO_BINARY_DESTINATION)Ported/$(TARGET)/$(NAME)-$(VERSION).zip ./* && \
	cd $(BACK) && \
	rm -rf $(PYRO_BINARY_DESTINATION)Ported/$(TARGET)/$(NAME)-$(VERSION) && \
	sync

clean:
	rm -rf $(NAME)-$(VERSION)
	rm -f $(NAME)-$(VERSION)-source.tar.gz
	rm -f $(JPEGNAME)src.v$(JPEGVERSION).tar.gz
	sync
