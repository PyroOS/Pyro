# Makefile for ported software.

BACK=`pwd`
URL = http://wormsofprey.org/download/
NAME = WormsOfPrey
DOWNLOADNAME = wop
VERSION = 0.4.3
DATAVERSION = 2005-12-21
DESC = Worms of Prey is the new and free real-time Worms game.
DOCS = AUTHORS ChangeLog COPYING README

dist: build inst doc update checksum package clean

build:
	wget -N $(URL)$(DOWNLOADNAME)-$(VERSION)-src.tar.bz2 && sync && \
	wget -N $(URL)$(DOWNLOADNAME)data-$(DATAVERSION).tar.bz2 && sync && \
	tar -xjkvf $(DOWNLOADNAME)-$(VERSION)-src.tar.bz2 && sync && \
	tar -xjkvf $(DOWNLOADNAME)data-$(DATAVERSION).tar.bz2 && sync && \
	cd $(DOWNLOADNAME)-$(VERSION) && \
	make && sync && \
	cd ..

inst:
	mkdir -p $(PYRO_BINARY_DESTINATION)Ported/$(TARGET)/$(NAME)-$(VERSION)/Applications/$(NAME)
	cp -rf $(DOWNLOADNAME)data-$(DATAVERSION)/* $(PYRO_BINARY_DESTINATION)Ported/$(TARGET)/$(NAME)-$(VERSION)/Applications/$(NAME)/
	cp -rf $(DOWNLOADNAME)-$(VERSION)/wop $(PYRO_BINARY_DESTINATION)Ported/$(TARGET)/$(NAME)-$(VERSION)/Applications/$(NAME)/Worms_Of_Prey
	addattrib $(PYRO_BINARY_DESTINATION)Ported/$(TARGET)/$(NAME)-$(VERSION)/Applications/$(NAME)/Worms_Of_Prey os::Category Games
	sync

doc:
	mkdir -p $(PYRO_BINARY_DESTINATION)Ported/$(TARGET)/$(NAME)-$(VERSION)/Applications/$(NAME)/Documentation
	for DOC in $(DOCS);do \
		cp -f $(ALTNAME)-$(ALTVERSION)/$$DOC $(PYRO_BINARY_DESTINATION)Ported/$(TARGET)/$(NAME)-$(VERSION)/Applications/$(NAME)/Documentation/; \
	done;
	for FILES in `find $(PYRO_BINARY_DESTINATION)Ported/$(TARGET)/$(NAME)-$(VERSION)/Applications/$(NAME)/Documentation/`; do \
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
	cp -f $(DOWNLOADNAME)-$(VERSION).tar.bz2 $(PYRO_BINARY_DESTINATION)Source/$(TARGET)/ && \
	cp -f $(DOWNLOADNAME)data-$(DATAVERSION).tar.bz2 $(PYRO_BINARY_DESTINATION)Source/$(TARGET)/ && \
	cd $(PYRO_BINARY_DESTINATION)Ported/$(TARGET)/$(NAME)-$(VERSION) && \
	zip -z < "$(TEMP)/zip_comment" -ry9 $(PYRO_BINARY_DESTINATION)Ported/$(TARGET)/$(NAME)-$(VERSION).zip ./* && \
	cd $(BACK) && \
	rm -rf $(PYRO_BINARY_DESTINATION)Ported/$(TARGET)/$(NAME)-$(VERSION) && \
	sync

clean:
	rm -rf $(DOWNLOADNAME)-$(VERSION)
	rm -rf $(DOWNLOADNAME)data-$(DATAVERSION)
	rm -f $(DOWNLOADNAME)-$(VERSION)-src.tar.bz2
	rm -f $(DOWNLOADNAME)data-$(DATAVERSION).tar.bz2
	sync
