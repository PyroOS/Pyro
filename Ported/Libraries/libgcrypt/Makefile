# Makefile for ported software.

BACK=`pwd`
URL = ftp://ftp.gnupg.org/gcrypt/libgcrypt/
NAME = libgcrypt
VERSION = 1.5.0
ARGS =	--prefix=$(PYRO_BINARY_DESTINATION)Ported/$(TARGET)/$(NAME)-$(VERSION)/NIX/$(NAME)
DESC = This is a general purpose cryptographic library based on the code from GnuPG.
DOCS = AUTHORS ChangeLog COPYING COPYING* INSTALL NEWS README THANKS TODO

dist: build doc update checksum package clean

build:
	wget --passive-ftp -N $(URL)$(NAME)-$(VERSION).tar.bz2 && sync && \
	tar -xjkvf $(NAME)-$(VERSION).tar.bz2 && sync && \
	cd $(NAME)-$(VERSION) && \
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
	cp -f $(NAME)-$(VERSION).tar.bz2 $(PYRO_BINARY_DESTINATION)Source/$(TARGET)/ && \
	cd $(PYRO_BINARY_DESTINATION)Ported/$(TARGET)/$(NAME)-$(VERSION) && \
	zip -z < "$(TEMP)/zip_comment" -ry9 $(PYRO_BINARY_DESTINATION)Ported/$(TARGET)/$(NAME)-$(VERSION).zip ./* && \
	cd $(BACK) && \
	rm -rf $(PYRO_BINARY_DESTINATION)Ported/$(TARGET)/$(NAME)-$(VERSION) && \
	sync

clean:
	rm -rf $(NAME)-$(VERSION)
	rm -f $(NAME)-$(VERSION).tar.bz2
	sync
