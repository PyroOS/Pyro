# Makefile for non-compilable items.

dist: template copy-to-admin home-attribute

template:
	mkdir -p $(PYRO_BINARY_DESTINATION)/System/resources/HomeTemplate
	cp -rf * $(PYRO_BINARY_DESTINATION)/System/resources/HomeTemplate/
	rm $(PYRO_BINARY_DESTINATION)/System/resources/HomeTemplate/Makefile
	addattrib $(PYRO_BINARY_DESTINATION)/System/resources/HomeTemplate os::Icon /boot/System/icons/home.png
	addattrib $(PYRO_BINARY_DESTINATION)/System/resources/HomeTemplate/Trash os::Icon /boot/System/icons/trash.png
	addattrib $(PYRO_BINARY_DESTINATION)/System/resources/HomeTemplate/Software/Applications os::Icon /boot/System/icons/applications.png
	sync

copy-to-admin:
	mkdir -p $(PYRO_BINARY_DESTINATION)/Home/admin
	cp -rf $(PYRO_BINARY_DESTINATION)/System/resources/HomeTemplate/* $(PYRO_BINARY_DESTINATION)/Home/admin/
	chown -R 0:0 $(PYRO_BINARY_DESTINATION)/Home/admin/
	sync

home-attribute:
	addattrib $(PYRO_BINARY_DESTINATION)/Home os::Icon /boot/System/icons/home.png
	sync

clean:
	sync
