# Makefile for ported software.

BACK=`pwd`
URL = http://sourceforge.net/projects/p7zip/files/p7zip/
NAME = 7zip
VERSION = 9.20.1
DESC = A file archiver with a high compression ratio.
DOCS = ChangeLog README TODO

dist: build inst doc update checksum package clean

build:
	wget -N $(URL)$(VERSION)/p$(NAME)_$(VERSION)_src_all.tar.bz2/download && sync && \
	tar -xjkvf p$(NAME)_$(VERSION)_src_all.tar.bz2 && sync && \
	cd p$(NAME)_$(VERSION) && \
	make && sync && \
	cd ..

inst:
	mkdir -p $(PYRO_BINARY_DESTINATION)Ported/$(TARGET)/$(NAME)-$(VERSION)/NIX/$(NAME)/bin
	cp -f p$(NAME)_$(VERSION)/bin/* $(PYRO_BINARY_DESTINATION)Ported/$(TARGET)/$(NAME)-$(VERSION)/NIX/$(NAME)/bin/

doc:
	mkdir -p $(PYRO_BINARY_DESTINATION)Ported/$(TARGET)/$(NAME)-$(VERSION)/Documentation/nix/$(NAME)
	for DOC in $(DOCS);do \
		cp -f p$(NAME)_$(VERSION)/$$DOC $(PYRO_BINARY_DESTINATION)Ported/$(TARGET)/$(NAME)-$(VERSION)/Documentation/nix/$(NAME)/; \
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
	cp -f p$(NAME)_$(VERSION)_src_all.tar.bz2 $(PYRO_BINARY_DESTINATION)Source/$(TARGET)/ && \
	cd $(PYRO_BINARY_DESTINATION)Ported/$(TARGET)/$(NAME)-$(VERSION) && \
	zip -z < "$(TEMP)/zip_comment" -ry9 $(PYRO_BINARY_DESTINATION)Ported/$(TARGET)/$(NAME)-$(VERSION).zip ./* && \
	cd $(BACK) && \
	rm -rf $(PYRO_BINARY_DESTINATION)Ported/$(TARGET)/$(NAME)-$(VERSION) && \
	sync

clean:
	rm -rf p$(NAME)_$(VERSION)
	rm -f p$(NAME)_$(VERSION)_src_all.tar.bz2
	sync
