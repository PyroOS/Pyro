# Makefile for non-compilable items.

dist: inst symlinks

inst:
	mkdir -p $(PYRO_BINARY_DESTINATION)/System/scripts
	cp -f * $(PYRO_BINARY_DESTINATION)/System/scripts/
	rm $(PYRO_BINARY_DESTINATION)/System/scripts/Makefile
	sync

symlinks:
	ln -sf /boot/System/scripts/updatepath.sh $(PYRO_BINARY_DESTINATION)/System/binary/updatepath
	chmod -x $(PYRO_BINARY_DESTINATION)/System/binary/updatepath

clean:
	sync
