# Makefile for non-compilable items.

dist: inst mimetypes checksum

inst:
	mkdir -p $(PYRO_BINARY_DESTINATION)/System/icons
	cp -r * $(PYRO_BINARY_DESTINATION)/System/icons/
	rm $(PYRO_BINARY_DESTINATION)/System/icons/Makefile
	sync

mimetypes:
	for FILES in `find $(PYRO_BINARY_DESTINATION)/System/icons/ -name *.png`; do \
		if test -f $$FILES; then \
			addattrib $$FILES "os::MimeType" "image/png"; \
		fi; \
	done;
	sync

checksum:
	for FILES in `find $(PYRO_BINARY_DESTINATION)/System/icons/`; do \
		if test -f $$FILES; then \
			addattrib $$FILES "CheckSum::MD5" `md5sum $$FILES | cut -c1-32`; \
		fi; \
	done;
	sync

clean:
	sync
