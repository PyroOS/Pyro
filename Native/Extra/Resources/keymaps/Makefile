# Makefile for non-compilable items.

BACK=`pwd`
DESC = Extra/alternate versions of keymaps.

dist: build checksum package

build:
	mkdir -p $(PYRO_BINARY_DESTINATION)Native/Extra/Resources/Keymaps/System/keymaps
	for keymap in *.map; do\
		destname=`basename $$keymap .map`;\
		rm -f $(PYRO_BINARY_DESTINATION)Native/Extra/Resources/Keymaps/System/keymaps/$$destname;\
		keymapconv $$destname.map $(PYRO_BINARY_DESTINATION)Native/Extra/Resources/Keymaps/System/keymaps/$$destname;\
	done
	sync

checksum:
	for FILES in `find $(PYRO_BINARY_DESTINATION)Native/Extra/Resources/Keymaps/System/keymaps/`; do \
		if test -f $$FILES; then \
			addattrib $$FILES "CheckSum::MD5" `md5sum $$FILES | cut -c1-32`; \
		fi; \
	done;
	sync

package:
	echo $(DESC) > $(TEMP)/zip_comment && \
	cd $(PYRO_BINARY_DESTINATION)Native/Extra/Resources/Keymaps && \
	zip -z < "$(TEMP)/zip_comment" -ry9 $(PYRO_BINARY_DESTINATION)Native/Extra/Resources/Keymaps.zip ./* && \
	cd $(BACK) && \
	rm -rf $(PYRO_BINARY_DESTINATION)Native/Extra/Resources/Keymaps && \
	sync

clean:
	sync
