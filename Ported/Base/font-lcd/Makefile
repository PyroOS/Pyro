# Makefile for non-compilable items.

BACK=`pwd`
URL = http://www.spinwardstars.com/scrfonts/zip/
NAME = lcd
DESC = A family of liquid-crystal-style typefaces.
DOCS = *.TXT

dist: download inst mimetypes doc checksum package clean

download:
	wget -N $(URL)$(NAME)wintt.zip && sync && \
	mkdir ./$(NAME)wintt && \
	unzip -n -d $(NAME)wintt $(NAME)wintt.zip ; sync && \
	sync

inst:
	mv lcdwintt/LCDWinTT/LCD-N___.TTF lcdwintt/LCDWinTT/LCD-Normal.ttf && \
	mv lcdwintt/LCDWinTT/LCD-L___.TTF lcdwintt/LCDWinTT/LCD-Light.ttf && \
	mv lcdwintt/LCDWinTT/LCD-BOLD.TTF lcdwintt/LCDWinTT/LCD-Bold.ttf && \
	mv lcdwintt/LCDWinTT/LCD-U___.TTF lcdwintt/LCDWinTT/LCD-Ultra.ttf && \
	mv lcdwintt/LCDWinTT/LCD2N___.TTF lcdwintt/LCDWinTT/LCD2-Normal.ttf && \
	mv lcdwintt/LCDWinTT/LCD2L___.TTF lcdwintt/LCDWinTT/LCD2-Light.ttf && \
	mv lcdwintt/LCDWinTT/LCD2B___.TTF lcdwintt/LCDWinTT/LCD2-Bold.ttf && \
	mv lcdwintt/LCDWinTT/LCD2U___.TTF lcdwintt/LCDWinTT/LCD2-Ultra.ttf && \
	mkdir -p $(PYRO_BINARY_DESTINATION)Ported/$(TARGET)/$(NAME)/System/fonts && sync && \
	cp -f $(NAME)wintt/LCDWinTT/*.ttf $(PYRO_BINARY_DESTINATION)Ported/$(TARGET)/$(NAME)/System/fonts/ && sync && \
	sync

mimetypes:
	for FILES in `find $(PYRO_BINARY_DESTINATION)Ported/$(TARGET)/$(NAME)/System/fonts/*`; do \
		if test -f $$FILES; then \
			addattrib $$FILES "os::MimeType" "application/x-font-ttf"; \
		fi; \
	done;
	sync

doc:
	mkdir -p $(PYRO_BINARY_DESTINATION)Ported/$(TARGET)/$(NAME)/Documentation/Resources/$(NAME)
	for DOC in $(DOCS);do \
		cp -f $(NAME)wintt/$$DOC $(PYRO_BINARY_DESTINATION)Ported/$(TARGET)/$(NAME)/Documentation/Resources/$(NAME)/; \
	done;
	for FILES in `find $(PYRO_BINARY_DESTINATION)Ported/$(TARGET)/$(NAME)/Documentation/Resources/$(NAME)/`; do \
		if test -f $$FILES; then \
			addattrib $$FILES "os::MimeType" "text/plain"; \
		fi; \
	done;
	sync

checksum:
	for FILES in `find $(PYRO_BINARY_DESTINATION)Ported/$(TARGET)/$(NAME)/`; do \
		if test -f $$FILES; then \
			addattrib $$FILES "CheckSum::MD5" `md5sum $$FILES | cut -c1-32`; \
		fi; \
	done;
	sync

package:
	echo $(DESC) > $(TEMP)/zip_comment && \
	mkdir -p $(PYRO_BINARY_DESTINATION)Source/$(TARGET) && \
	cp -f $(NAME)wintt.zip $(PYRO_BINARY_DESTINATION)Source/$(TARGET)/ && \
	cd $(PYRO_BINARY_DESTINATION)Ported/$(TARGET)/$(NAME) && \
	zip -z < "$(TEMP)/zip_comment" -ry9 $(PYRO_BINARY_DESTINATION)Ported/$(TARGET)/$(NAME).zip ./* && \
	cd $(BACK) && \
	rm -rf $(PYRO_BINARY_DESTINATION)Ported/$(TARGET)/$(NAME) && \
	sync

clean:
	rm -rf $(NAME)wintt
	rm -f $(NAME)wintt.zip
	sync
