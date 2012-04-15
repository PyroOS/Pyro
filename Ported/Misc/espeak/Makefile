# Makefile for ported software.

BACK=`pwd`
URL = http://sourceforge.net/projects/espeak/files/espeak/
NAME = espeak
VERSION = 1.45
RELEASE = 05
ARGS =	DESTDIR=$(PYRO_BINARY_DESTINATION)Source/$(TARGET)/$(NAME)-$(VERSION)/NIX/$(NAME)
DESC = Speech Synthesizer.
DOCS = ReadMe *.txt

dist: build doc update checksum package clean

build:
	wget -N $(URL)$(NAME)-$(VERSION)/$(NAME)-$(VERSION).$(RELEASE)-source.zip/download && sync && \
	unzip -n $(NAME)-$(VERSION).$(RELEASE)-source.zip && sync && \
	cd $(NAME)-$(VERSION).$(RELEASE)-source/src && \
	make $(ARGS) install && sync && \
	cd ..

doc:
	mkdir -p $(PYRO_BINARY_DESTINATION)Source/$(TARGET)/$(NAME)-$(VERSION)/Documentation/nix/$(NAME)
	for DOC in $(DOCS);do \
		cp -f $(NAME)-$(VERSION).$(RELEASE)-source/$$DOC $(PYRO_BINARY_DESTINATION)Source/$(TARGET)/$(NAME)-$(VERSION)/Documentation/nix/$(NAME)/; \
	done;
	for FILES in `find $(PYRO_BINARY_DESTINATION)Source/$(TARGET)/$(NAME)-$(VERSION)/Documentation/`; do \
		if test -f $$FILES; then \
			addattrib $$FILES "os::MimeType" "text/plain"; \
		fi; \
	done;
	sync

update:
	for FILES in `find $(PYRO_BINARY_DESTINATION)Source/$(TARGET)/$(NAME)-$(VERSION)/NIX/`; do \
		if [ `file --brief --mime-type $$FILES | grep -c "text" -` != 0 ]; then \
			sed -i "s,$(PYRO_BINARY_DESTINATION)Source/$(TARGET)/$(NAME)-$(VERSION),/boot,g" $$FILES; \
		fi; \
	done;
	sync

checksum:
	for FILES in `find $(PYRO_BINARY_DESTINATION)Source/$(TARGET)/$(NAME)-$(VERSION)/`; do \
		if test -f $$FILES; then \
			addattrib $$FILES "CheckSum::MD5" `md5sum $$FILES | cut -c1-32`; \
		fi; \
	done;
	sync

package:
	echo $(DESC) > $(TEMP)/zip_comment && \
	mkdir -p $(PYRO_BINARY_DESTINATION)Source/$(TARGET) && \
	cp -f $(NAME)-$(VERSION).$(RELEASE)-source.zip $(PYRO_BINARY_DESTINATION)Source/$(TARGET)/ && \
	cd $(PYRO_BINARY_DESTINATION)Source/$(TARGET)/$(NAME)-$(VERSION) && \
	zip -z < "$(TEMP)/zip_comment" -ry9 $(PYRO_BINARY_DESTINATION)Source/$(TARGET)/$(NAME)-$(VERSION).zip ./* && \
	cd $(BACK) && \
	rm -rf $(PYRO_BINARY_DESTINATION)Source/$(TARGET)/$(NAME)-$(VERSION) && \
	sync

clean:
	rm -rf $(NAME)-$(VERSION).$(RELEASE)-source
	rm -f $(NAME)-$(VERSION).$(RELEASE)-source.zip
	sync
