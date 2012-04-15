# Makefile for non-compilable items.

dist: inst doc

inst:
	mkdir -p $(PYRO_BINARY_DESTINATION)Native/Development/System/binary
	cp -f sindent.sh $(PYRO_BINARY_DESTINATION)Native/Development/System/binary/
	chmod +x $(PYRO_BINARY_DESTINATION)Native/Development/System/binary/sindent.sh
	addattrib $(PYRO_BINARY_DESTINATION)Native/Development/System/binary/sindent.sh "CheckSum::MD5" `md5sum $(PYRO_BINARY_DESTINATION)Native/Development/System/binary/sindent.sh | cut -c1-32`
	sync

doc:
	mkdir -p $(PYRO_BINARY_DESTINATION)Native/Development/Documentation/Development/sident
	cp -f README $(PYRO_BINARY_DESTINATION)Native/Development/Documentation/Development/sident/
	addattrib $(PYRO_BINARY_DESTINATION)Native/Development/Documentation/Development/sident/README "os::MimeType" "text/plain"
	addattrib $(PYRO_BINARY_DESTINATION)Native/Development/Documentation/Development/sident/README "CheckSum::MD5" `md5sum $(PYRO_BINARY_DESTINATION)Native/Development/Documentation/Development/sident/README | cut -c1-32`; \
	sync

clean:
	sync
