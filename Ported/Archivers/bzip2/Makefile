# Makefile for ported software.

BACK=`pwd`
URL = http://www.bzip.org/
NAME = bzip2
VERSION = 1.0.6
DESC = A freely available, patent free, high-quality data compressor.
DOCS = CHANGES LICENSE manual.* README README*

dist: build inst doc update checksum package clean

build:
	wget -N $(URL)$(VERSION)/$(NAME)-$(VERSION).tar.gz && sync && \
	tar -xzkvf $(NAME)-$(VERSION).tar.gz && sync && \
	cd $(NAME)-$(VERSION) && \
	make -f Makefile-libbz2_so && sync && \
	make install PREFIX=$(PYRO_BINARY_DESTINATION)Ported/$(TARGET)/$(NAME)-$(VERSION)/NIX/$(NAME) && sync && \
	cd ..

inst:
	mkdir -p $(PYRO_BINARY_DESTINATION)Ported/$(TARGET)/$(NAME)-$(VERSION)/NIX/lib
	cp -f $(NAME)-$(VERSION)/libbz2.so.$(VERSION) $(PYRO_BINARY_DESTINATION)Ported/$(TARGET)/$(NAME)-$(VERSION)/NIX/lib/
	ln -sf ./libbz2.so.$(VERSION) $(PYRO_BINARY_DESTINATION)Ported/$(TARGET)/$(NAME)-$(VERSION)/NIX/lib/libbz2.so.1.0
	ln -sf ./libbz2.so.$(VERSION) $(PYRO_BINARY_DESTINATION)Ported/$(TARGET)/$(NAME)-$(VERSION)/NIX/lib/libbz2.so

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
	cp -f $(NAME)-$(VERSION).tar.gz $(PYRO_BINARY_DESTINATION)Source/$(TARGET)/ && \
	cd $(PYRO_BINARY_DESTINATION)Ported/$(TARGET)/$(NAME)-$(VERSION) && \
	zip -z < "$(TEMP)/zip_comment" -ry9 $(PYRO_BINARY_DESTINATION)Ported/$(TARGET)/$(NAME)-$(VERSION).zip ./* && \
	cd $(BACK) && \
	rm -rf $(PYRO_BINARY_DESTINATION)Ported/$(TARGET)/$(NAME)-$(VERSION) && \
	sync

clean:
	rm -rf $(NAME)-$(VERSION)
	rm -f $(NAME)-$(VERSION).tar.gz
	sync
