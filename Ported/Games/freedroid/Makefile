# Makefile for ported software.

BACK=`pwd`
URL = http://sourceforge.net/projects/freedroid/files/freedroidRPG/
NAME = freedroid
VERSION = 0.15
ARGS =	--prefix=$(PYRO_BINARY_DESTINATION)Ported/$(TARGET)/$(NAME)-$(VERSION)/NIX/$(NAME)
DESC = The game tells the story of a world destroyed by a conflict between robots and their human masters. Play as Tux in a quest to save the world from the murderous rebel bots who know no mercy. You get to choose which path you wish to follow, and freedom of choice is everywhere in the game.
DOCS = AABOUT-NLS AUTHORS ChangeLog HELP_WANTED INSTALL NEWS README

dist: build doc update checksum package clean

build:
	wget -N $(URL)$(NAME)RPG-$(VERSION)/$(NAME)rpg-$(VERSION).tar.gz/download && sync && \
	tar -xzkvf $(NAME)rpg-$(VERSION).tar.gz && sync && \
	cd $(NAME)rpg-$(VERSION) && \
	./configure $(ARGS) && sync && \
	make && make install && sync && \
	cd ..

doc:
	mkdir -p $(PYRO_BINARY_DESTINATION)Ported/$(TARGET)/$(NAME)-$(VERSION)/Documentation/nix/$(NAME)
	for DOC in $(DOCS);do \
		cp -f $(NAME)rpg-$(VERSION)/$$DOC $(PYRO_BINARY_DESTINATION)Ported/$(TARGET)/$(NAME)-$(VERSION)/Documentation/nix/$(NAME)/; \
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
	cp -f $(NAME)rpg-$(VERSION).tar.gz $(PYRO_BINARY_DESTINATION)Source/$(TARGET)/ && \
	cd $(PYRO_BINARY_DESTINATION)Ported/$(TARGET)/$(NAME)-$(VERSION) && \
	zip -z < "$(TEMP)/zip_comment" -ry9 $(PYRO_BINARY_DESTINATION)Ported/$(TARGET)/$(NAME)-$(VERSION).zip ./* && \
	cd $(BACK) && \
	rm -rf $(PYRO_BINARY_DESTINATION)Ported/$(TARGET)/$(NAME)-$(VERSION) && \
	sync

clean:
	rm -rf $(NAME)rpg-$(VERSION)
	rm -f $(NAME)rpg-$(VERSION).tar.gz
	sync
