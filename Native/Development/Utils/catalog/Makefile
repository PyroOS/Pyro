CFLAGS	+= -c
CXXFLAGS += -c
OBJDIR = objs

EXES	= $(OBJDIR)/catalogtool $(OBJDIR)/catcomp

catalogtool_OBJS = catalogtool.o ccatalog.o
catcomp_OBJS = catcomp.o ccatalog.o

catalogtool_OBJS	:= $(addprefix $(OBJDIR)/,$(catalogtool_OBJS))
catcomp_OBJS	:= $(addprefix $(OBJDIR)/,$(catcomp_OBJS))

include $(PYRO_COMMON)/makefile.rules

all: $(OBJDIR) $(EXES)

$(OBJDIR):
	-mkdir $(OBJDIR)

$(OBJDIR)/catcomp: $(catcomp_OBJS)
	$(CXX) $(catcomp_OBJS) -lpyro -o $@
	rescopy -r $(OBJDIR)/catcomp resources/catcomp.catalog
	for CATALOG in `ls -1 resources`;do \
		if [ -d "resources/$$CATALOG" ] ; then \
			echo $$CATALOG; \
			rescopy -a $(OBJDIR)/catcomp $$CATALOG/catcomp.catalog=resources/$$CATALOG/catcomp.catalog; \
		fi \
	done;

$(OBJDIR)/catalogtool: $(catalogtool_OBJS)
	$(CXX) $(catalogtool_OBJS) -lsyllable -o $@

# ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

dist: catalogs all inst doc web clean

catalogs:
	cd resources; \
	catcomp -c catcomp.cd; \
	for CATALOG in `ls -1`;do \
		if [ -d "$$CATALOG" ] ; then \
			echo $$CATALOG; \
			cd $$CATALOG; \
			catcomp -t catcomp.cd; \
			cd ..; \
		fi \
	done;
	sync

inst:
	mkdir -p $(PYRO_BINARY_DESTINATION)Native/Development/System/binary
	cp -f $(EXES) $(PYRO_BINARY_DESTINATION)Native/Development/System/binary/
	addattrib $(PYRO_BINARY_DESTINATION)Native/Development/System/binary/catalogtool "CheckSum::MD5" `md5sum $(PYRO_BINARY_DESTINATION)Native/Development/System/binary/catalogtool | cut -c1-32`
	addattrib $(PYRO_BINARY_DESTINATION)Native/Development/System/binary/catcomp "CheckSum::MD5" `md5sum $(PYRO_BINARY_DESTINATION)Native/Development/System/binary/catcomp | cut -c1-32`
	sync

doc:
	mkdir -p $(PYRO_BINARY_DESTINATION)Native/Development/Documentation/Development/catalog
	cp -f README $(PYRO_BINARY_DESTINATION)Native/Development/Documentation/Development/catalog/
	addattrib $(PYRO_BINARY_DESTINATION)Native/Development/Documentation/Development/catalog/README "os::MimeType" "text/plain"
	addattrib $(PYRO_BINARY_DESTINATION)Native/Development/Documentation/Development/catalog/README "CheckSum::MD5" `md5sum $(PYRO_BINARY_DESTINATION)Native/Development/Documentation/Development/catalog/README | cut -c1-32`
	sync

web:
	mkdir -p $(PYRO_BINARY_DESTINATION)Web/Languages/LIST
	mkdir -p $(PYRO_BINARY_DESTINATION)Web/Languages/Development/catalog
	cd resources; \
	for CATALOG in `ls -1`;do \
		if [ -d "$$CATALOG" ] ; then \
			touch $(PYRO_BINARY_DESTINATION)Web/Languages/LIST/$$CATALOG; \
			touch $(PYRO_BINARY_DESTINATION)Web/Languages/Development/catalog/int; \
			touch $(PYRO_BINARY_DESTINATION)Web/Languages/Development/catalog/$$CATALOG; \
		fi \
	done;
	sync

clean: catalog-clean
	rm -rf $(OBJDIR)
	rm -f $(EXES)
	sync

catalog-clean:
	rm -f resources/catcomp.catalog resources/catcomp.h
	for CATALOG in `ls -1 resources`;do \
		rm -f resources/$$CATALOG/catcomp.catalog; \
	done;
	sync
