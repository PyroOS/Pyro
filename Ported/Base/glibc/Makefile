# Makefile for ported software.

BACK=`pwd`
URL = http://ftp.gnu.org/gnu/glibc/
NAME = glibc
VERSION = 2.7
ARGS =	--prefix=$(PYRO_BINARY_DESTINATION)Ported/$(TARGET)/$(NAME)-$(VERSION)/NIX/$(NAME) \
		--enable-hacker-mode \
		--enable-add-ons=syllable \
		--sysconfdir=/boot/System/config/etc \
		--without-cvs \
		--without-__thread \
		--with-arch=i586 \
#		--enable-add-ons=libidn \
DESC = GNU C Library.
DOCS = BUGS ChangeLog ChangeLog* CONFORMANCE COPYING COPYING* FAQ INSTALL LICENSES NEWS NOTES PROJECTS README syllable/Changes syllable/Changes* syllable/README syllable/TODO

dist: build doc update checksum package clean

build:
	mkdir -p $(NAME)-build && \
	wget -N $(URL)$(NAME)-$(VERSION).tar.gz && sync && \
	wget -N $(URL)$(NAME)-libidn-$(VERSION).tar.gz && sync && \
	tar -xzkvf $(NAME)-$(VERSION).tar.gz && sync && \
	cd $(NAME)-$(VERSION) && \
#	tar -xzkvf ../$(NAME)-libidn-$(VERSION).tar.gz && sync && \
#	mv -v $(NAME)-libidn-$(VERSION) libidn && \
	patch --dry-run	--strip=1 < ../PATCH/$(NAME).patch && \
	patch --strip=1 < ../PATCH/$(NAME).patch && sync && \
	cp -a ../PATCH/* ./ && sync && \
	sed -i '/vi_VN.TCVN/d' localedata/SUPPORTED && sync && \
	cd ../$(NAME)-build && \
	../$(NAME)-$(VERSION)/configure $(ARGS) && sync && \
	make -r PARALLELMFLAGS="" CVSOPTS="" -C ../$(NAME)-$(VERSION) objdir=`pwd` all && make install && sync && \
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
	sync

update:
	for FILES in `find $(PYRO_BINARY_DESTINATION)Ported/$(TARGET)/$(NAME)-$(VERSION)/System/`; do \
		if [ `file --brief --mime-type $$FILES | grep -c "text" -` != 0 ]; then \
			sed -i "s,$(PYRO_BINARY_DESTINATION)Ported/$(TARGET)/$(NAME)-$(VERSION),/boot,g" $$FILES; \
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
	cp -f $(NAME)-$(VERSION).tar.gz $(PYRO_BINARY_DESTINATION)Source/$(TARGET)/ && \
#	cp -f $(NAME)-libidn-$(VERSION).tar.gz $(PYRO_BINARY_DESTINATION)Source/$(TARGET)/ && \
	cd $(PYRO_BINARY_DESTINATION)Ported/$(TARGET)/$(NAME)-$(VERSION) && \
	zip -z < "$(TEMP)/zip_comment" -ry9 $(PYRO_BINARY_DESTINATION)Ported/$(TARGET)/$(NAME)-$(VERSION).zip ./* && \
	cd $(BACK) && \
	rm -rf $(PYRO_BINARY_DESTINATION)Ported/$(TARGET)/$(NAME)-$(VERSION) && \
	sync

clean:
	rm -rf $(NAME)-build
	rm -rf $(NAME)-$(VERSION)
	rm -f $(NAME)-$(VERSION).tar.gz
	rm -f $(NAME)-libidn-$(VERSION).tar.gz
	sync
