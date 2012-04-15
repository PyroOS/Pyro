# Makefile for ported software.

BACK=`pwd`
URL = http://ftp.gnu.org/gnu/gcc/
NAME = gcc
VERSION = 4.1.2
ARGS =	--prefix=$(PYRO_BINARY_DESTINATION)Ported/$(TARGET)/$(NAME)-$(VERSION)/NIX/$(NAME) \
		--enable-languages=c,c++ \
		--enable-sjlj-execptions \
		--enable-shared \
		--enable-threads \
		--disable-libstdcxx-pch \
		--with-system-zlib \
		--with-arch=i586
DESC = The GNU Compiler Collection.
DOCS = ABOUT-NLS BUGS ChangeLog ChangeLog* COPYING COPYING* FAQ LAST_UPDATED MAINTAINERS NEWS README *.html

dist: build doc update copy-to-base checksum package clean

build:
	wget -N $(URL)$(NAME)-$(VERSION)/$(NAME)-$(VERSION).tar.bz2 && sync && \
	tar -xjkvf $(NAME)-$(VERSION).tar.bz2 && sync && \
	cd $(NAME)-$(VERSION) && \
	patch --dry-run	--strip=1 < ../PATCH/$(NAME).patch && \
	patch --strip=1 < ../PATCH/$(NAME).patch && sync && \
	cp -vrf ../PATCH/* ./ && sync && \
	./configure $(ARGS) && sync && \
	make && make install && sync && \
	cd ..

doc:
	mkdir -p $(PYRO_BINARY_DESTINATION)Ported/$(TARGET)/$(NAME)-$(VERSION)/Documentation/nix/$(NAME)
	for DOC in $(DOCS);do \
		cp -f $(NAME)-$(VERSION)/$$DOC $(PYRO_BINARY_DESTINATION)Ported/$(TARGET)/$(NAME)-$(VERSION)/Documentation/nix/$(NAME)/; \
	done;
	for FILES in `find $(PYRO_BINARY_DESTINATION)Ported/$(TARGET)/$(NAME)-$(VERSION)/Documentation/`; do \
		if test -f $$FILES; then \
			addattrib $$FILES "os::MimeType" "text/plain"; \
		fi; \
	done;
	for FILES in `find $(PYRO_BINARY_DESTINATION)Ported/$(TARGET)/$(NAME)-$(VERSION)/Documentation/ -name "*html"`; do \
		if test -f $$FILES; then \
			addattrib $$FILES "os::MimeType" "text/html"; \
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

copy-to-base:
	mkdir -p $(PYRO_BINARY_DESTINATION)Ported/Base/$(NAME)-$(VERSION)/NIX/$(NAME)/lib && \
	cp -f $(PYRO_BINARY_DESTINATION)Ported/$(TARGET)/$(NAME)-$(VERSION)/NIX/$(NAME)/lib/*.so $(PYRO_BINARY_DESTINATION)Ported/Base/$(NAME)-$(VERSION)/NIX/$(NAME)/lib/ && sync && \
	cp -f $(PYRO_BINARY_DESTINATION)Ported/$(TARGET)/$(NAME)-$(VERSION)/NIX/$(NAME)/lib/*.so.* $(PYRO_BINARY_DESTINATION)Ported/Base/$(NAME)-$(VERSION)/NIX/$(NAME)/lib/ && sync && \
	mkdir -p $(PYRO_BINARY_DESTINATION)Ported/Base/$(NAME)-$(VERSION)/System/nix/lib && \
	ln -s /boot/NIX/gcc/lib/libgcc_s.so $(PYRO_BINARY_DESTINATION)Ported/Base/$(NAME)-$(VERSION)/System/nix/lib/libgcc_s.so.3 && \
	ln -s /boot/NIX/gcc/lib/libssp.so $(PYRO_BINARY_DESTINATION)Ported/Base/$(NAME)-$(VERSION)/System/nix/lib/libssp.so && \
	ln -s /boot/NIX/gcc/lib/libstdc++.so $(PYRO_BINARY_DESTINATION)Ported/Base/$(NAME)-$(VERSION)/System/nix/lib/libstdc++.so && sync && \
	sync

checksum:
	for FILES in `find $(PYRO_BINARY_DESTINATION)Ported/$(TARGET)/$(NAME)-$(VERSION)/`; do \
		if test -f $$FILES; then \
			addattrib $$FILES "CheckSum::MD5" `md5sum $$FILES | cut -c1-32`; \
		fi; \
	done;
	for FILES in `find $(PYRO_BINARY_DESTINATION)Ported/Base/$(NAME)-$(VERSION)/`; do \
		if test -f $$FILES; then \
			addattrib $$FILES "CheckSum::MD5" `md5sum $$FILES | cut -c1-32`; \
		fi; \
	done;
	sync

package:
	echo $(DESC) > $(TEMP)/zip_comment && \
	mkdir -p $(PYRO_BINARY_DESTINATION)Source/$(TARGET) && \
	cp -f $(NAME)-$(VERSION).tar.bz2 $(PYRO_BINARY_DESTINATION)Source/$(TARGET)/ && \
	cd $(PYRO_BINARY_DESTINATION)Ported/$(TARGET)/$(NAME)-$(VERSION) && \
	zip -z < "$(TEMP)/zip_comment" -ry9 $(PYRO_BINARY_DESTINATION)Ported/$(TARGET)/$(NAME)-$(VERSION).zip ./* && \
	cd $(BACK) && \
	rm -rf $(PYRO_BINARY_DESTINATION)Ported/$(TARGET)/$(NAME)-$(VERSION) && \
	sync
	cd $(PYRO_BINARY_DESTINATION)Ported/Base/$(NAME)-$(VERSION) && \
	zip -z < "$(TEMP)/zip_comment" -ry9 $(PYRO_BINARY_DESTINATION)Ported/Base/$(NAME)-$(VERSION).zip ./* && \
	cd $(BACK) && \
	rm -rf $(PYRO_BINARY_DESTINATION)Ported/Base/$(NAME)-$(VERSION) && \
	sync

clean:
	rm -rf $(NAME)-build
	rm -rf $(NAME)-$(VERSION)
	rm -f $(NAME)-$(VERSION).tar.bz2
	sync
