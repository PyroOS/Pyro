# Makefile for non-compilable items.

dist: inst mimetypes checksum

inst:
	mkdir -p $(PYRO_BINARY_DESTINATION)/System/resources/HomeTemplate/Pictures
	for DIR in `ls -1 ./` ; do \
		if test -d $$DIR; then \
			cp -f $$DIR/* $(PYRO_BINARY_DESTINATION)/System/resources/HomeTemplate/Pictures/ ; \
		fi; \
	done;

mimetypes:
	for FILES in `find $(PYRO_BINARY_DESTINATION)/System/resources/HomeTemplate/Pictures/ -name *.png`; do \
		if test -f $$FILES; then \
			addattrib $$FILES "os::MimeType" "image/png"; \
		fi; \
	done;
	for FILES in `find $(PYRO_BINARY_DESTINATION)/System/resources/HomeTemplate/Pictures/ -name *.gif`; do \
		if test -f $$FILES; then \
			addattrib $$FILES "os::MimeType" "image/gif"; \
		fi; \
	done;
	for FILES in `find $(PYRO_BINARY_DESTINATION)/System/resources/HomeTemplate/Pictures/ -name *.jpeg`; do \
		if test -f $$FILES; then \
			addattrib $$FILES "os::MimeType" "image/jpeg"; \
		fi; \
	done;
	for FILES in `find $(PYRO_BINARY_DESTINATION)/System/resources/HomeTemplate/Pictures/ -name *.tiff`; do \
		if test -f $$FILES; then \
			addattrib $$FILES "os::MimeType" "image/tiff"; \
		fi; \
	done;
	sync

checksum:
	for FILES in `find $(PYRO_BINARY_DESTINATION)/System/resources/HomeTemplate/Pictures/`; do \
		if test -f $$FILES; then \
			addattrib $$FILES "CheckSum::MD5" `md5sum $$FILES | cut -c1-32`; \
		fi; \
	done;
	sync

clean:
	sync
