# Makefile for non-compilable items.

dist: extra-dirs symlinks attributes

extra-dirs:
	mkdir -p $(PYRO_BINARY_DESTINATION)/System/framework
	mkdir -p $(PYRO_BINARY_DESTINATION)/System/temp
	mkdir -p $(PYRO_BINARY_DESTINATION)/System/log
	mkdir -p $(PYRO_BINARY_DESTINATION)/System/nix/bin
	mkdir -p $(PYRO_BINARY_DESTINATION)/System/nix/include
	mkdir -p $(PYRO_BINARY_DESTINATION)/System/nix/lib/pkgconfig
	mkdir -p $(PYRO_BINARY_DESTINATION)/System/nix/sbin
	mkdir -p $(PYRO_BINARY_DESTINATION)/System/nix/share/man
	mkdir -p $(PYRO_BINARY_DESTINATION)/System/nix/var/cache
	mkdir -p $(PYRO_BINARY_DESTINATION)/System/nix/var/log
	mkdir -p $(PYRO_BINARY_DESTINATION)/System/nix/var/spool
	mkdir -p $(PYRO_BINARY_DESTINATION)/System/nix/var/tmp
	mkdir -p $(PYRO_BINARY_DESTINATION)/Software/Applications
	mkdir -p $(PYRO_BINARY_DESTINATION)/Software/binary
	mkdir -p $(PYRO_BINARY_DESTINATION)/Software/libraries
	mkdir -p $(PYRO_BINARY_DESTINATION)/Software/NIX
	mkdir -p $(PYRO_BINARY_DESTINATION)/NIX
	sync

symlinks:
	ln -sf /boot/System/config/etc $(PYRO_BINARY_DESTINATION)/System/nix/etc

attributes:
	addattrib $(PYRO_BINARY_DESTINATION)/Software/Applications os::Icon /boot/System/icons/applications.png
	addattrib $(PYRO_BINARY_DESTINATION)/Applications os::Icon /boot/System/icons/applications.png
	addattrib $(PYRO_BINARY_DESTINATION)/Preferences os::Icon /boot/System/icons/settings.png
	sync

clean:
	sync
