#Makefile for TableView test app
COPTS = -c -fexceptions -ffast-math

OBJS = DemoApp.o tableview.o

OBJDIR := objs
OBJS	:= $(addprefix $(OBJDIR)/,$(OBJS))

# Rules
$(OBJDIR)/%.o : %.c
	@echo Compiling : $<
	@$(CC) $(COPTS) $< -o $@

$(OBJDIR)/%.o : %.cpp
	@echo Compiling : $<
	@$(CXX) $(COPTS) $< -o $@

$(OBJDIR)/%.o : %.s
	@echo Assembling : $<
	@$(CC) $(COPTS) -x assembler-with-cpp $< -o $@

all : objs tabletest

objs:
	@mkdir -p objs

tabletest: $(OBJS)
	@echo Linking...
	@$(CXX) $(OBJS) -o DemoApp -lpyro

# ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

BACK=`pwd`
DESC = TableView replaces TableView found in libpyro with one based on the same algorithm as used in GTK.

dist: all inst checksum package clean

inst:
	mkdir -p $(PYRO_BINARY_DESTINATION)Native/Extra/Demo/TableView/
	cp -rf ./* $(PYRO_BINARY_DESTINATION)Native/Extra/Demo/TableView/
	rm -rf $(PYRO_BINARY_DESTINATION)Native/Extra/Demo/TableView/objs
	sync

checksum:
	for FILES in `find $(PYRO_BINARY_DESTINATION)Native/Extra/Demo/TableView/`; do \
		if test -f $$FILES; then \
			addattrib $$FILES "CheckSum::MD5" `md5sum $$FILES | cut -c1-32`; \
		fi; \
	done;
	sync

package:
	echo $(DESC) > $(TEMP)/zip_comment && \
	cd $(PYRO_BINARY_DESTINATION)Native/Extra/Demo && \
	zip -z < "$(TEMP)/zip_comment" -ry9 $(PYRO_BINARY_DESTINATION)Native/Extra/Demo/TableView.zip ./TableView && \
	cd $(BACK) && \
	rm -rf $(PYRO_BINARY_DESTINATION)Native/Extra/Demo/TableView && \
	sync

clean:
	rm -rf $(OBJDIR)
	rm -f DemoApp
	sync
