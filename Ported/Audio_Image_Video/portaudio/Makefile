# Makefile for ported software.

BACK=`pwd`
URL = http://www.portaudio.com/archives/
NAME = portaudio
VERSION = 19
RELEASE = 20110326
ARGS =	--prefix=$(PYRO_BINARY_DESTINATION)Ported/$(TARGET)/$(NAME)-$(VERSION)_$(RELEASE)/NIX/$(NAME)
DESC = A cross platform, audio I/O library.
DOCS = *.txt

dist: build doc update checksum package clean

build:
	wget -N $(URL)pa_stable_v$(VERSION)_$(RELEASE).tgz && sync && \
	tar -xzkvf pa_stable_v$(VERSION)_$(RELEASE).tgz && sync && \
	cd $(NAME) && \
	./configure $(ARGS) && sync && \
	make && make install && sync && \
	cd ..

doc:
	mkdir -p $(PYRO_BINARY_DESTINATION)Ported/$(TARGET)/$(NAME)-$(VERSION)_$(RELEASE)/Documentation/nix/$(NAME)
	for DOC in $(DOCS);do \
		cp -f $(NAME)/$$DOC $(PYRO_BINARY_DESTINATION)Ported/$(TARGET)/$(NAME)-$(VERSION)_$(RELEASE)/Documentation/nix/$(NAME)/; \
	done;
	for FILES in `find $(PYRO_BINARY_DESTINATION)Ported/$(TARGET)/$(NAME)-$(VERSION)_$(RELEASE)/Documentation/`; do \
		if test -f $$FILES; then \
			addattrib $$FILES "os::MimeType" "text/plain"; \
		fi; \
	done;
	sync

update:
	for FILES in `find $(PYRO_BINARY_DESTINATION)Ported/$(TARGET)/$(NAME)-$(VERSION)_$(RELEASE)/NIX/`; do \
		if [ `file --brief --mime-type $$FILES | grep -c "text" -` != 0 ]; then \
			sed -i "s,$(PYRO_BINARY_DESTINATION)Ported/$(TARGET)/$(NAME)-$(VERSION)_$(RELEASE),/boot,g" $$FILES; \
		fi; \
	done;
	sync

checksum:
	for FILES in `find $(PYRO_BINARY_DESTINATION)Ported/$(TARGET)/$(NAME)-$(VERSION)_$(RELEASE)/`; do \
		if test -f $$FILES; then \
			addattrib $$FILES "CheckSum::MD5" `md5sum $$FILES | cut -c1-32`; \
		fi; \
	done;
	sync

package:
	echo $(DESC) > $(TEMP)/zip_comment && \
	mkdir -p $(PYRO_BINARY_DESTINATION)Source/$(TARGET) && \
	cp -f pa_stable_v$(VERSION)_$(RELEASE).tgz $(PYRO_BINARY_DESTINATION)Source/$(TARGET)/$(NAME)-$(VERSION).tar.gz && \
	cd $(PYRO_BINARY_DESTINATION)Ported/$(TARGET)/$(NAME)-$(VERSION)_$(RELEASE) && \
	zip -z < "$(TEMP)/zip_comment" -ry9 $(PYRO_BINARY_DESTINATION)Ported/$(TARGET)/$(NAME)-$(VERSION)_$(RELEASE).zip ./* && \
	cd $(BACK) && \
	rm -rf $(PYRO_BINARY_DESTINATION)Ported/$(TARGET)/$(NAME)-$(VERSION)_$(RELEASE) && \
	sync

clean:
	rm -rf $(NAME)
	rm -f pa_stable_v$(VERSION)_$(RELEASE).tgz
	sync
