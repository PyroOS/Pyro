COPTS	= -Wall -c

OBJS	= postoffice.o DemoApp.o
EXE		= DemoApp

CC	= gcc
LL	= gcc

$(EXE) : $(OBJS)
	$(LL) $(OBJS) -lpyro -lstdc++ -o $@

#.c.o :
#	$(CC) $(COPTS) $< -o $@
.cpp.o:
	$(CC) $(COPTS) $< -o $@

postoffice.o:	postoffice.cpp postoffice.h
DemoApp.o:			DemoApp.cpp postoffice.h

# ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

BACK=`pwd`
DESC = PostOffice is a singleton class intended to help you pass messages between windows and/or other Looper-derived classes.

dist: $(EXE) inst checksum package clean

inst:
	mkdir -p $(PYRO_BINARY_DESTINATION)Native/Extra/Demo/PostOffice/
	cp -f ./* $(PYRO_BINARY_DESTINATION)Native/Extra/Demo/PostOffice/
	rm -f $(PYRO_BINARY_DESTINATION)Native/Extra/Demo/PostOffice/*.o
	addattrib $(PYRO_BINARY_DESTINATION)Native/Extra/Demo/PostOffice/README "os::MimeType" "text/plain"
	sync

checksum:
	for FILES in `find $(PYRO_BINARY_DESTINATION)Native/Extra/Demo/PostOffice/`; do \
		if test -f $$FILES; then \
			addattrib $$FILES "CheckSum::MD5" `md5sum $$FILES | cut -c1-32`; \
		fi; \
	done;
	sync

package:
	echo $(DESC) > $(TEMP)/zip_comment && \
	cd $(PYRO_BINARY_DESTINATION)Native/Extra/Demo && \
	zip -z < "$(TEMP)/zip_comment" -ry9 $(PYRO_BINARY_DESTINATION)Native/Extra/Demo/PostOffice.zip ./PostOffice && \
	cd $(BACK) && \
	rm -rf $(PYRO_BINARY_DESTINATION)Native/Extra/Demo/PostOffice && \
	sync

clean:
	rm -f $(OBJS)
	rm -f $(EXE)
	sync
