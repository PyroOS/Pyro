# Makefile for ported software.

BACK=`pwd`
URL = ftp://ftp.berlios.de/pub/cdrecord/alpha/
NAME = cdrtools
VERSION = 3.01
RELEASE = a06
ARGS =	$(PYRO_BINARY_DESTINATION)Ported/$(TARGET)/$(NAME)-$(VERSION)/
DESC = Cdrtools is a set of command line programs that allows to record CD/DVD/BluRay media.
DOCS = AN-$(VERSION)$(RELEASE) Changelog Changelog* CONTRIBUTING COPYING README *.txt
DOCS_mkisofs = ChangeLog ChangeLog* COPYING README README*
DOCS_cdda2wave = FAQ HOWTOUSE NEEDED THANKS
DOCS_cdrecord = COPYING README README* 
MODULES = mkisofs 

dist: build doc update checksum package clean

build:
	wget --passive-ftp -N $(URL)$(NAME)-$(VERSION)$(RELEASE).tar.gz && sync && \
	tar -xzkvf $(NAME)-$(VERSION)$(RELEASE).tar.gz ; sync && \
	cd $(NAME)-$(VERSION) && \
	make GMAKE_NOWARN=true INS_BASE=/NIX/$(NAME) DESTDIR=$(ARGS) install && sync && \
	cd ..

doc:
	mkdir -p $(PYRO_BINARY_DESTINATION)Ported/$(TARGET)/$(NAME)-$(VERSION)/Documentation/nix/$(NAME)
	for DOC in $(DOCS);do \
		cp -f $(NAME)-$(VERSION)/$$DOC $(PYRO_BINARY_DESTINATION)Ported/$(TARGET)/$(NAME)-$(VERSION)/Documentation/nix/$(NAME)/; \
	done;
	for MODULE in $(MODULES);do \
		mkdir -p $(PYRO_BINARY_DESTINATION)Ported/$(TARGET)/$(NAME)-$(VERSION)/Documentation/nix/$(NAME)/$$MODULE; \
		for MDOC in $(DOCS_$$MODULE);do \
			cp -f $(NAME)-$(VERSION)/$$MODULE/$$MDOC $(PYRO_BINARY_DESTINATION)Ported/$(TARGET)/$(NAME)-$(VERSION)/Documentation/nix/$(NAME)/$$MODULE/; \
		done; \
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
	cp -f $(NAME)-$(VERSION)$(RELEASE).tar.gz $(PYRO_BINARY_DESTINATION)Source/$(TARGET)/ && \
	cd $(PYRO_BINARY_DESTINATION)Ported/$(TARGET)/$(NAME)-$(VERSION) && \
	zip -z < "$(TEMP)/zip_comment" -ry9 $(PYRO_BINARY_DESTINATION)Ported/$(TARGET)/$(NAME)-$(VERSION)$(RELEASE).zip ./* && \
	cd $(BACK) && \
	rm -rf $(PYRO_BINARY_DESTINATION)Ported/$(TARGET)/$(NAME)-$(VERSION) && \
	sync

clean:
	rm -rf $(NAME)-$(VERSION)
	rm -f $(NAME)-$(VERSION)$(RELEASE).tar.gz
	sync
