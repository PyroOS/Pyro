# Makefile for non-compilable items.

dist: inst checksum

inst:
	mkdir -p $(PYRO_BINARY_DESTINATION)/System/keymaps
	for keymap in *.map; do\
		destname=`basename $$keymap .map`;\
		rm -f $(PYRO_BINARY_DESTINATION)/System/keymaps/$$destname;\
		keymapconv $$destname.map $(PYRO_BINARY_DESTINATION)/System/keymaps/$$destname;\
	done
	sync

checksum:
	for FILES in `find $(PYRO_BINARY_DESTINATION)/System/keymaps/`; do \
		if test -f $$FILES; then \
			addattrib $$FILES "CheckSum::MD5" `md5sum $$FILES | cut -c1-32`; \
		fi; \
	done;
	sync

clean:
	sync
