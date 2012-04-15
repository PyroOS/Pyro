CFLAGS   += -c -fexceptions
CXXFLAGS += -c -fexceptions -I.
AOPTS    = -c

LIBBIN = $(PYRO_BINARY_DESTINATION)/System/libraries

LIBNAME = libpyro.so.1

VPATH = ./ gui storage util translation gui/colorselector gui/fontrequester

OBJS	= rect.o region.o desktop.o bitmap.o view.o tabview.o			\
	  window.o slider.o scrollbar.o spinner.o frameview.o			\
	  checkbox.o textview.o button.o font.o stdbitmaps.o			\
	  control.o stringview.o menu.o tableview.o sprite.o progressbar.o	\
	  listview.o requesters.o layoutview.o			\
	  radiobutton.o filerequester.o dropdownmenu.o checkmenu.o splitter.o \
	  image.o imageview.o imagebutton.o treeview.o \
      popupmenu.o iconview.o icondirview.o statusbar.o toolbar.o separator.o inputbox.o

OBJS += colorselector.o colorview.o colorwheel.o gradientview.o
OBJS += fontrequester.o fontrequesterview.o
OBJS += colorrequester.o

OBJS += appserverconfig.o looper.o handler.o application.o message.o messenger.o messagequeue.o	\
	invoker.o clipboard.o string.o regexp.o variant.o circularbuffer.o	\
	resources.o optionparser.o settings.o thread.o messagefilter.o event.o
OBJS += filereference.o filereference_info.o filereference_rename.o operation_copy.o operation_move.o operation_delete.o fsnode.o streamableio.o \
	seekableio.o file.o tempfile.o directory.o directory_create.o symlink.o nodemonitor.o path.o memfile.o registrar.o volumes.o \
	datetime.o shortcutkey.o catalog.o keymap.o \
	locale.o
OBJS += translator.o
OBJS += vstubs.o

OBJDIR	:= objs

include $(PYRO_COMMON)/makefile.rules

all :	$(OBJDIR) $(OBJDIR)/$(LIBNAME)

$(OBJDIR):
	mkdir $(OBJDIR)

$(OBJDIR)/$(LIBNAME): $(OBJS)
	$(CXX) -shared -Xlinker "-soname=$(LIBNAME)" $(OBJS) -lm -o $(OBJDIR)/$(LIBNAME)

deps: $(OBJDIR) $(DEPS)

-include $(OBJDIR)/*.d

# ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

dist: catalog all catalog-inst inst doc web clean

catalog:
	cd catalogs; \
	catcomp -c libsyllable.cd; \
	for CATALOG in `ls -1`;do \
		if [ -d "$$CATALOG" ] ; then \
			echo $$CATALOG; \
			cd $$CATALOG; \
			catcomp -t libsyllable.cd; \
			cd ..; \
		fi \
	done;
	sync

catalog-inst:
	mkdir -p $(PYRO_BINARY_DESTINATION)/System/resources/catalogs
	cp -f catalogs/libsyllable.catalog $(PYRO_BINARY_DESTINATION)/System/resources/catalogs/
	addattrib $(PYRO_BINARY_DESTINATION)/System/resources/catalogs/libsyllable.catalog "CheckSum::MD5" `md5sum $(PYRO_BINARY_DESTINATION)/System/resources/catalogs/libsyllable.catalog | cut -c1-32`
	for CATALOG in `ls -1 catalogs`;do \
		if [ -d "catalogs/$$CATALOG" ] ; then \
			echo $$CATALOG; \
			mkdir -p $(PYRO_BINARY_DESTINATION)/System/resources/catalogs/$$CATALOG; \
			cp catalogs/$$CATALOG/libsyllable.catalog $(PYRO_BINARY_DESTINATION)/System/resources/catalogs/$$CATALOG/; \
			addattrib $(PYRO_BINARY_DESTINATION)/System/resources/catalogs/$$CATALOG/libsyllable.catalog "CheckSum::MD5" `md5sum $(PYRO_BINARY_DESTINATION)/System/resources/catalogs/$$CATALOG/libsyllable.catalog | cut -c1-32`; \
		fi \
	done
	sync

inst:
	mkdir -p $(PYRO_BINARY_DESTINATION)/System/libraries
	mkdir -p $(PYRO_BINARY_DESTINATION)/System/resources/catalogs
	cp -f $(OBJDIR)/$(LIBNAME) $(PYRO_BINARY_DESTINATION)/System/libraries/
	ln -sf $(LIBNAME) $(PYRO_BINARY_DESTINATION)/System/libraries/libpyro.so
	addattrib $(PYRO_BINARY_DESTINATION)/System/libraries/$(LIBNAME) "CheckSum::MD5" `md5sum $(PYRO_BINARY_DESTINATION)/System/libraries/$(LIBNAME) | cut -c1-32`
	addattrib $(PYRO_BINARY_DESTINATION)/System/libraries/$(LIBNAME) "os::Icon" "/System/icons/logo.png"
	addattrib $(PYRO_BINARY_DESTINATION)/System/libraries/$(LIBNAME) "os::MimeType" "pyro/library"
	sync

doc:
	mkdir -p docs
	doxygen libpyro.dox
	mkdir -p $(PYRO_BINARY_DESTINATION)Native/Development/Documentation/API/LibPyro
	cp -rf docs/html/* $(PYRO_BINARY_DESTINATION)Native/Development/Documentation/API/LibPyro/
	sync

web:
	mkdir -p $(PYRO_BINARY_DESTINATION)Web/Documentation/API/LibPyro
	cp -rf docs/html/* $(PYRO_BINARY_DESTINATION)Web/Documentation/API/LibPyro/
	mkdir -p $(PYRO_BINARY_DESTINATION)Web/Languages/LIST
	mkdir -p $(PYRO_BINARY_DESTINATION)Web/Languages/Libraries/libpyro
	cd catalogs; \
	for CATALOG in `ls -1`;do \
		if [ -d "$$CATALOG" ] ; then \
			touch $(PYRO_BINARY_DESTINATION)Web/Languages/LIST/$$CATALOG; \
			touch $(PYRO_BINARY_DESTINATION)Web/Languages/Libraries/libpyro/int; \
			touch $(PYRO_BINARY_DESTINATION)Web/Languages/Libraries/libpyro/$$CATALOG; \
		fi \
	done;
	sync

clean: catalog-clean
	rm -rf $(OBJDIR)
	rm -rf docs
	sync

catalog-clean:
	rm -f catalogs/libsyllable.catalog catalogs/libsyllable.h
	for CATALOG in `ls -1 catalogs`;do \
		rm -f catalogs/$$CATALOG/libsyllable.catalog; \
	done;
	sync
