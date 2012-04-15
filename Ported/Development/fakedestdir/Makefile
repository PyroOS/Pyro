# Makefile for ported software.

BACK=`pwd`
URL = http://tukaani.org/fakedestdir/
NAME = fakedestdir
DESC = Written to ease packaging software, whose makefiles don't support DESTDIR or any equivalent feature.

dist: build inst doc checksum package clean

build:
	mkdir $(NAME) && \
	cd $(NAME) && \
	wget -N $(URL)$(NAME) && sync && \
	wget -N $(URL) && sync && \
	chmod +x $(NAME) && \
	mv index.html $(NAME).html
	cd ..

inst:
	mkdir -p $(PYRO_BINARY_DESTINATION)Ported/$(TARGET)/$(NAME)/NIX/$(NAME)/bin
	cp -f $(NAME)/$(NAME) $(PYRO_BINARY_DESTINATION)Ported/$(TARGET)/$(NAME)/NIX/$(NAME)/bin/

doc:
	mkdir -p $(PYRO_BINARY_DESTINATION)Ported/$(TARGET)/$(NAME)/Documentation/nix/$(NAME)
	cp -f $(NAME)/$(NAME).html $(PYRO_BINARY_DESTINATION)Ported/$(TARGET)/$(NAME)/Documentation/nix/$(NAME)/; \
	addattrib $(PYRO_BINARY_DESTINATION)Ported/$(TARGET)/$(NAME)/Documentation/nix/$(NAME)/$(NAME).html "os::MimeType" "text/html"; \

checksum:
	for FILES in `find $(PYRO_BINARY_DESTINATION)Ported/$(TARGET)/$(NAME)/`; do \
		if test -f $$FILES; then \
			addattrib $$FILES "CheckSum::MD5" `md5sum $$FILES | cut -c1-32`; \
		fi; \
	done;
	sync

package:
	echo $(DESC) > $(TEMP)/zip_comment && \
	cd $(PYRO_BINARY_DESTINATION)Ported/$(TARGET)/$(NAME) && \
	zip -z < "$(TEMP)/zip_comment" -ry9 $(PYRO_BINARY_DESTINATION)Ported/$(TARGET)/$(NAME).zip ./* && \
	cd $(BACK) && \
	rm -rf $(PYRO_BINARY_DESTINATION)Ported/$(TARGET)/$(NAME) && \
	sync

clean:
	rm -rf $(NAME)
	sync
