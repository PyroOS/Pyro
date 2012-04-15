# Makefile for non-compilable items.

BACK=`pwd`
URL = https://fedorahosted.org/releases/l/i/liberation-fonts/
NAME = liberation
VERSION = 1.07.1
DESC = A font family which aims at metric compatibility with Arial, Times New Roman, and Courier New.
DOCS = AUTHORS ChangeLog COPYING README TODO *.txt

dist: download inst mimetypes doc checksum package clean

download:
	wget -N $(URL)$(NAME)-fonts-ttf-$(VERSION).tar.gz && sync && \
	tar -xzkvf $(NAME)-fonts-ttf-$(VERSION).tar.gz && sync && \
	sync

inst:
	mkdir -p $(PYRO_BINARY_DESTINATION)Ported/$(TARGET)/$(NAME)-$(VERSION)/System/fonts && \
	cp -f $(NAME)-fonts-ttf-$(VERSION)/*.ttf $(PYRO_BINARY_DESTINATION)Ported/$(TARGET)/$(NAME)-$(VERSION)/System/fonts/ && sync && \
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
		cp -f $(NAME)-fonts-ttf-$(VERSION)/$$DOC $(PYRO_BINARY_DESTINATION)Ported/$(TARGET)/$(NAME)-$(VERSION)/Documentation/Resources/$(NAME)/; \
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
	cp -f $(NAME)-fonts-ttf-$(VERSION).tar.gz $(PYRO_BINARY_DESTINATION)Source/$(TARGET)/ && \
	cd $(PYRO_BINARY_DESTINATION)Ported/$(TARGET)/$(NAME)-$(VERSION) && \
	zip -z < "$(TEMP)/zip_comment" -ry9 $(PYRO_BINARY_DESTINATION)Ported/$(TARGET)/$(NAME)-$(VERSION).zip ./* && \
	cd $(BACK) && \
	rm -rf $(PYRO_BINARY_DESTINATION)Ported/$(TARGET)/$(NAME)-$(VERSION) && \
	sync

clean:
	rm -rf $(NAME)-fonts-ttf-$(VERSION)
	rm -f $(NAME)-fonts-ttf-$(VERSION).tar.gz
	sync
