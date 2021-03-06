# Makefile for ported software.

BACK=`pwd`
URL = http://sourceforge.net/projects/wesnoth/files/wesnoth-1.8/
NAME = wesnoth
RELEASENAME=Battle_for_Wesnoth
VERSION = 1.8.6
ARGS =	--prefix=$(PYRO_BINARY_DESTINATION)Ported/$(TARGET)/$(RELEASENAME)-$(VERSION)/NIX/$(NAME)
DESC = The Battle for Wesnoth is a turn-based tactical strategy game with a high fantasy theme, featuring both single-player, and online/hotseat multiplayer combat. Fight a desperate battle to reclaim the throne of Wesnoth, or take hand in any number of other adventures.
DOCS = AUTHORS ChangeLog COPYING THANKS

dist: build doc update package clean

build:
	wget -N $(URL)$(NAME)-$(VERSION)/$(NAME)-$(VERSION).tar.bz2/download && sync && \
	tar -xjkvf $(NAME)-$(VERSION).tar.bz2 && sync && \
	cd $(NAME)-$(VERSION) && \
	./autogen.sh $(ARGS) && sync && \
	./configure $(ARGS) && sync && \
	make && make install && sync && \
	cd ..

doc:
	mkdir -p $(PYRO_BINARY_DESTINATION)Ported/$(TARGET)/$(RELEASENAME)-$(VERSION)/Documentation/nix/$(RELEASENAME)
	for DOC in $(DOCS);do \
		cp -f $(NAME)-$(VERSION)/$$DOC $(PYRO_BINARY_DESTINATION)Ported/$(TARGET)/$(RELEASENAME)-$(VERSION)/Documentation/nix/$(RELEASENAME)/; \
	done;
	for FILES in `find $(PYRO_BINARY_DESTINATION)Ported/$(TARGET)/$(RELEASENAME)-$(VERSION)/Documentation/`; do \
		if test -f $$FILES; then \
			addattrib $$FILES "os::MimeType" "text/plain"; \
		fi; \
	done;
	sync

update:
	for FILES in `find $(PYRO_BINARY_DESTINATION)Ported/$(TARGET)/$(RELEASENAME)-$(VERSION)/NIX/`; do \
		if [ `file --brief --mime-type $$FILES | grep -c "text" -` != 0 ]; then \
			sed -i "s,$(PYRO_BINARY_DESTINATION)Ported/$(TARGET)/$(RELEASENAME)-$(VERSION),/boot,g" $$FILES; \
		fi; \
	done;
	sync

checksum:
	for FILES in `find $(PYRO_BINARY_DESTINATION)Ported/$(TARGET)/$(RELEASENAME)-$(VERSION)/`; do \
		if test -f $$FILES; then \
			addattrib $$FILES "CheckSum::MD5" `md5sum $$FILES | cut -c1-32`; \
		fi; \
	done;
	sync

package:
	echo $(DESC) > $(TEMP)/zip_comment && \
	mkdir -p $(PYRO_BINARY_DESTINATION)Source/$(TARGET) && \
	cp -f $(NAME)-$(VERSION).tar.bz2 $(PYRO_BINARY_DESTINATION)Source/$(TARGET)/ && \
	cd $(PYRO_BINARY_DESTINATION)Ported/$(TARGET)/$(RELEASENAME)-$(VERSION) && \
	zip -z < "$(TEMP)/zip_comment" -ry9 $(PYRO_BINARY_DESTINATION)Ported/$(TARGET)/$(RELEASENAME)-$(VERSION).zip ./* && \
	cd $(BACK) && \
	rm -rf $(PYRO_BINARY_DESTINATION)Ported/$(TARGET)/$(RELEASENAME)-$(VERSION) && \
	sync

clean:
	rm -rf $(NAME)-$(VERSION)
	rm -f $(NAME)-$(VERSION).tar.bz2
	sync
